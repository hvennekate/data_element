#include "specspectrumplot.h"
#include "actionlib/specplotmovecommand.h"
#include "actionlib/specplotmovecommand.h"
#include "utility-functions.h"

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
	specPlot(parent),
	view(0),
	picker(0)
{
	correctionActions = new QActionGroup(this) ;
	correctionActions->setExclusive(false);
	offsetAction  = new QAction(QIcon(":/offset.png"), "Offset", correctionActions) ;
	offlineAction = new QAction(QIcon(":/offline.png"), "Slope",correctionActions) ;
	scaleAction   = new QAction(QIcon(":/scale.png"),"Scale", correctionActions) ;
	shiftAction   = new QAction("shift x",correctionActions) ;

	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;
	correctionActions->addAction(shiftAction) ;

	connect(correctionActions,SIGNAL(triggered(QAction*)),this,SLOT(correctionsChanged())) ;
	foreach(QAction *action, correctionActions->actions())
		action->setCheckable(true) ;

	alignmentActions = new QActionGroup(this) ;
	alignmentActions->setExclusive(false) ;
	setReferenceAction = new QAction("set reference",alignmentActions) ;
	alignWithReferenceAction = new QAction("align",alignmentActions) ;
	alignWithReferenceAction->setCheckable(true) ;
	alignWithReferenceAction->setChecked(false) ;
	addRangeAction = new QAction("new range",alignmentActions) ;
	removeRangeAction = new QAction("delete range",alignmentActions) ;
	noSlopeAction = new QAction("no slope",alignmentActions) ;
	noSlopeAction->setCheckable(true) ;
	noSlopeAction->setChecked(false) ;

	alignmentActions->addAction(setReferenceAction) ;
	alignmentActions->addAction(alignWithReferenceAction) ;
	alignmentActions->addAction(addRangeAction) ;
	alignmentActions->addAction(removeRangeAction) ;
	alignmentActions->addAction(noSlopeAction) ;


	connect(alignmentActions,SIGNAL(triggered(QAction*)),this,SLOT(alignmentChanged(QAction*))) ;
}

void specSpectrumPlot::alignmentChanged(QAction *action)
{
	if (action == setReferenceAction)
	{
	}
	else if (action == alignWithReferenceAction)
	{
		if (!alignWithReferenceAction->isChecked())
		{
			if (picker)
			{
				disconnect(picker, SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SLOT(applyZeroRanges(specCanvasItem*,int,double,double))) ;
				delete picker ;
				picker = 0 ;
			}
		}
		correctionActions->setDisabled(alignWithReferenceAction->isChecked());
		if (!picker && alignWithReferenceAction->isChecked())
		{
			picker = new CanvasPicker(this) ;
			picker->setOwning() ;
			connect(picker, SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SLOT(applyZeroRanges(specCanvasItem*,int,double,double))) ;
		}
		// TODO disable/enable highlighting of ranges or plots  ...
		// !alignWithReferenceAction->isChecked() && correctionsChecked()
		// or maybe simply call correctionsChanged()
	}
	else if (action == addRangeAction)
	{
		double min = axisScaleDiv(QwtPlot::xBottom)->lowerBound(), max = axisScaleDiv(QwtPlot::xBottom)->upperBound() ;
		specRange *newRange = new specRange(min+.1*(max-min),max-.1*(max-min)) ;
		newRange->attach(this) ;
		picker->addSelectable(newRange) ;
	}
	else if (action == removeRangeAction)
		picker->removeSelected();
}

bool specSpectrumPlot::correctionChecked()
{
	bool checked = false ;
	foreach(QAction *action, correctionActions->actions())
		checked = checked || action->isChecked() ;
	return checked ;
}

void specSpectrumPlot::correctionsChanged()
{
	if (!correctionChecked() || !correctionActions->isEnabled())
	{
		// TODO unset highlighting -- should be done by picker...
		qDebug("removing picker") ;
		if (picker)
		{
			delete picker ;
			picker = 0 ;
		}
		pointHash.clear();
		// TODO merge undo stack actions -- dangerous:  consider other actions might have been performed meanwhile
//		disconnect(undoPartner,SIGNAL(stackChanged()),this,SLOT(replot())) ;
	}
	else
	{
		if (!picker)
		{
			qDebug("installing picker") ;
			picker = new CanvasPicker(this) ;
			connect(picker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this,SLOT(pointMoved(specCanvasItem*,int,double,double))) ;
			QList<specCanvasItem*> items ;
			QwtPlotItemList onPlot = itemList(spec::spectrum) ;
			foreach(QwtPlotItem* item, onPlot)
				items << (specCanvasItem*) item ;
			picker->addSelectable(items);
		}
//		connect(undoPartner,SIGNAL(stackChanged()),this,SLOT(replot())) ;
		// TODO set highlighting
	}
}

void specSpectrumPlot::pointMoved(specCanvasItem *item, int no, double x, double y)
{
	if (!view) return ;
	// get reference to point list from point hash (in order to re-use old code below)
	qDebug("processing moved point") ;
	QList<int>& selectedPoints = pointHash[item] ;
	// Add new Point to list.
	selectedPoints.prepend(selectedPoints.contains(no) ? selectedPoints.takeAt(selectedPoints.indexOf(no)) : no) ;

	// TODO take care of the case where there is no undo stack available...
	if (!undoPartner) return ;

	// collect correction parameters
	// x shift:
	double shift = shiftAction->isChecked() ? x-item->sample(no).x() : 0 ;

	// compute other corrections:
	// TODO rewrite code for GSL
	qDebug("creating matrix") ;
	QList<QList<double> > matrix ;
	for (int i = 0 ; i < selectedPoints.size() ; i++) matrix << QList<double>() ;
	if (scaleAction->isChecked())
		for (int i = 0 ; i < selectedPoints.size(); i++)
			matrix[i] << item->sample(selectedPoints[i]).y() ;
	if (offsetAction->isChecked())
		for (int i = 0 ; i < selectedPoints.size(); i++)
			matrix[i] << 1. ;
	if (offlineAction->isChecked())
		for (int i = 0 ; i < selectedPoints.size(); i++)
			matrix[i] << item->sample(selectedPoints[i]).x()+shift ;
	for (int i = 0 ; i < matrix.size() ; i++) // verringere, wenn noetig, die Spaltenzahl
		while (matrix.size() < matrix[i].size())
			matrix[i].takeLast() ;
	while(matrix[0].size() < matrix.size()) // verringere, wenn noetig, die Zeilenzahl
		matrix.takeLast() ;

	qDebug("computing coeffs") ;
	QList<double> coeffs ;
	if (!matrix.isEmpty())
	{
		QList<double> yVals ;
		yVals << y- (scaleAction->isChecked() ? 0. : item->sample(no).y()) ; // TODO verify!
		for (int i = 1 ; i < selectedPoints.size() && i < matrix.size() ; i++)
			yVals << (scaleAction->isChecked() ? item->sample(selectedPoints[i]).y() : 0 ) ;

		// do coefficient calculation
		coeffs = gaussjinv(matrix,yVals) ;
	}

	double scale = scaleAction->isChecked() && !coeffs.isEmpty() ? coeffs.takeFirst() : 1. ,
	       offset= offsetAction->isChecked()&& !coeffs.isEmpty() ? coeffs.takeFirst() : 0. ,
	       offline=offlineAction->isChecked()&&!coeffs.isEmpty() ?coeffs.takeFirst() : 0. ;

	qDebug("pushing new move command %f %f %f %f",shift,offset,offline,scale) ;
	specPlotMoveCommand *command = new specPlotMoveCommand ;
	command->setItem(view->model()->index( (specModelItem*) item)) ; // TODO do dynamic cast first!!
	command->setCorrections(shift,offset,offline,scale) ;
	command->setParentWidget(view) ;
	undoPartner->push(command) ;
}

void specSpectrumPlot::applyZeroRanges(specCanvasItem* item,int point, double newX, double newY)
{
	((specRange*) item)->pointMoved(point,newX,newY) ;
	QwtPlotItemList zeroRanges = itemList(spec::zeroRange) ;
	QwtPlotItemList spectra = itemList(spec::spectrum) ; // TODO roll back previous undo command if it was of the same kind and merge with what is to come.
	specUndoCommand *zeroCommand = new specUndoCommand ; // TODO establish extra class
	for (int i = 0 ; i < spectra.size() ; ++i)
	{
		QList<QPointF> pointsInRange ;
		specModelItem* spectrum = (specModelItem*) spectra[i] ;
		// extract points that lie in any of the ranges
		for (int j = 0 ; j < spectrum->dataSize() ; ++j)
		{
			QPointF point = spectrum->sample(j) ;
			for (int k = 0 ; k < zeroRanges.size() ; ++k)
			{
				specRange* range = (specRange*) zeroRanges[k] ;
				if (range->contains(point.x()))
				{
					pointsInRange << point ;
					break ; // only include each point once
				}
			}
		}
		// compute correction
		double offset = 0, slope = 0 ;
		if((noSlopeAction->isChecked() && !pointsInRange.isEmpty() )|| pointsInRange.size() == 1)
		{
			// offset only
			qDebug("computing offset") ;
			for (int j = 0 ; j < pointsInRange.size() ; ++j)
				offset += pointsInRange[j].y() ;
			offset /= pointsInRange.size() ;
			qDebug("done computing offset") ;
		}
		else if (pointsInRange.size() > 1)
		{
			// slope and offset
			// should really be done using GSL
			QList<double> vector(QVector<double>(2,0.).toList()) ;
			QList<QList<double> > matrix(QVector<QList<double> >(2,vector).toList()) ;
			for (int j = 0 ; j < pointsInRange.size() ; ++j)
			{
				double x = pointsInRange[j].x(), y = pointsInRange[j].y() ;
				vector[0] += y ;
				vector[1] += y*x ;
				matrix[0][0] += x ;
				matrix[0][1] ++ ;
				matrix[1][0] += x*x ;
				matrix[1][1] += x ;
			}

			if (matrix[0][1] > 1.)
			{
				QList<double> correction = gaussjinv(matrix,vector) ;
				offset = correction[1];
				slope = correction[0] ;
			}
		}
		specPlotMoveCommand *command = new specPlotMoveCommand(zeroCommand) ;
		command->setItem(view->model()->index(spectrum));
		qDebug() << "setting slope and offset" << offset << slope ;
		command->setCorrections(0,-offset,-slope,1.) ;
		command->setParentWidget(view) ;
	}
	undoPartner->push(zeroCommand) ;
	replot() ;
}
