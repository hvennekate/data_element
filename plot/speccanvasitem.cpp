#include "speccanvasitem.h"
#include "utility-functions.h"
#include "specplot.h"
#include <QTextStream>
#include <QInputDialog>

specCanvasItem::specCanvasItem( QString description)
		: QwtPlotCurve(description)
{
}

void specCanvasItem::pointMoved(const int& no, const double& x, const double& y)
{
	// Add new Point to list.
	selectedPoints.prepend(selectedPoints.contains(no) ? selectedPoints.takeAt(selectedPoints.indexOf(no)) : no) ;
	// Shrink list to max. three elements
	while (selectedPoints.size() > 3)
		selectedPoints.takeLast() ;
	
	spec::moveMode mode = (dynamic_cast<specPlot*>(plot()) ? ((specPlot*) plot())->moveMode() : spec::none );

	// Apply x correction if necessary.
	if(mode & spec::shiftX)
	{
		moveXBy(x-QwtPlotCurve::sample(no).x()) ;
		refreshPlotData() ; // Alternatively:  replace last element in Matrix with new x value.
	}

	if(mode != spec::shiftX)
	{
		// Build Matrix and vector for coefficient calculation
		QList<QList<double> > matrix ;
		for (int i = 0 ; i < selectedPoints.size() ; i++) matrix << QList<double>() ;
		if (mode & spec::scale)
			for (int i = 0 ; i < selectedPoints.size(); i++)
				matrix[i] << QwtPlotCurve::sample(selectedPoints[i]).y() ;
		if (mode & spec::shift)
			for (int i = 0 ; i < selectedPoints.size(); i++)
				matrix[i] << 1. ;
		if (mode & spec::slope)
			for (int i = 0 ; i < selectedPoints.size(); i++)
				matrix[i] << QwtPlotCurve::sample(selectedPoints[i]).x() ;
		for (int i = 0 ; i < matrix.size() ; i++) // verringere, wenn noetig, die Spaltenzahl
			while (matrix.size() < matrix[i].size())
				matrix[i].takeLast() ;
		while(matrix[0].size() < matrix.size()) // verringere, wenn noetig, die Zeilenzahl
			matrix.takeLast() ;

		QList<double> yVals ;
		yVals << y- (mode & spec::scale ? 0. : QwtPlotCurve::sample(no).y()) ;
		for (int i = 1 ; i < selectedPoints.size() && i < matrix.size() ; i++)
			yVals << (mode & spec::scale ? QwtPlotCurve::sample(selectedPoints[i]).y() : 0 ) ;
	
		// do coefficient calculation
		QList<double> coeffs = gaussjinv(matrix,yVals) ;
	
		// apply coefficient changes
		if((mode & spec::scale) && !coeffs.empty())
			scaleBy(coeffs.takeFirst()) ;
		if((mode & spec::shift) && !coeffs.empty())
			moveYBy(coeffs.takeFirst()) ;
		if((mode & spec::slope) && !coeffs.empty())
			addToSlope(coeffs.takeFirst()) ;
	}
	
	refreshPlotData() ;
	plot()->replot() ;
}

void specCanvasItem::applyRanges(QList<QwtInterval*>& ranges)
{
	QList<double> vector ;
	QList<QList<double> > matrix ;
	vector << 0. << 0. ;
	matrix << QList<double>(vector) << QList<double>(vector) ;
	for(int i = 0 ; i < ranges.size() ; i++)
	{
		for (int j = 0 ; j < dataSize() ; j++)
		{
			if (ranges[i]->contains(sample(j).x()))
			{
				double x = QwtPlotCurve::sample(j).x(), y = QwtPlotCurve::sample(j).y() ;
				vector[0] += y ;
				vector[1] += y*x ;
				matrix[0][0] += x ;
				matrix[0][1] ++ ;
				matrix[1][0] += x*x ;
				matrix[1][1] += x ;
			}
		}
	}
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "Vector:  " << vector[0] << "  " << vector[1] << endl ;
// 	cout << "Matrix:  " << matrix[0][0] << "  " << matrix[0][1] << endl ;
// 	cout << "Matrix:  " << matrix[1][0] << "  " << matrix[1][1] << endl ;
// 	cout << "Ranges:  " << ranges.size() << "  data:  " << dataSize() << endl ;
// 	for (int i = 0 ; i < ranges.size() ; i++)
// 		cout << "Range " << i << ":  " << ranges[i]->minValue() << " - " << ranges[i]->maxValue() << endl ;
	
	if (matrix[0][1] > 1.)
	{
		QList<double> correction = gaussjinv(matrix,vector) ;
// 		QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 		cout << "Correction:  " << correction[1] << "  " << correction[0] << endl ;
		moveYBy(-correction[1]) ;
		addToSlope(-correction[0]) ;
		refreshPlotData() ;
	}
}

specCanvasItem::~specCanvasItem()
{
	specPlot *pointer = (specPlot*) plot() ;
	detach() ;
	if (pointer) pointer->replot() ;
}

void specCanvasItem::setLineWidth()
{
	bool ok = false ; // TODO: QTextEdit
	double newWidth = QInputDialog::getDouble((QWidget*) plot(),"Line width","New line width:",pen().widthF() , 0., 200.,1., &ok) ;
	if (ok)
	{
		QPen newPen(pen()) ;
		newPen.setWidthF(newWidth) ;
		setPen(newPen) ;
		if (plot()) plot()->replot() ;
	}
}

QMenu* specCanvasItem::contextMenu()
{ return 0 ;} // TODO
