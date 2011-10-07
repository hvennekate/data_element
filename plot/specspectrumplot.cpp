#include "specspectrumplot.h"
#include "actionlib/specplotmovecommand.h"
#include "actionlib/specplotmovecommand.h"

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
	specPlot(parent),
	model(0)
{
	offsetAction  = new QAction(QIcon(":/offset.png"), "Offset", this) ;
	offlineAction = new QAction(QIcon(":/offline.png"), "Slope",this) ;
	scaleAction   = new QAction(QIcon(":/scale.png"),"Scale", this) ;
	shiftAction   = new QAction(QIcon(":/shift.png"),"shift x",this) ;

	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;
	correctionActions->addAction(shiftAction) ;

	connect(correctionActions,SIGNAL(triggered(QAction*)),this,SLOT(correctionsChanged())) ;
	foreach(QAction *action, correctionActions->actions())
		action->setCheckable(true) ;
}

void specSpectrumPlot::correctionsChanged()
{
	bool checked = false ;
	foreach(QAction *action, correctionActions->actions())
		checked = checked || action->isChecked() ;
	if (!checked)
	{
		// TODO unset highlighting
		if (picker)
		{
			delete picker ;
			picker = 0 ;
		}
		pointHash.clear();
		// TODO merge undo stack actions
	}
	else
	{
		picker = new CanvasPicker(this) ;
		connect(picker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)),this,SLOT(pointMoved(specCanvasItem*,int,double,double))) ;
		// TODO set highlighting
	}
}

void specSpectrumPlot::pointMoved(specCanvasItem *item, int no, double x, double y)
{
	if (!model) return ;
	// get reference to point list from point hash (in order to re-use old code below)
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

	if (!matrix.isEmpty())
	{
		QList<double> yVals ;
		yVals << y- (scaleAction->isChecked() ? 0. : item->sample(no).y()) ; // TODO verify!
		for (int i = 1 ; i < selectedPoints.size() && i < matrix.size() ; i++)
			yVals << (scaleAction->isChecked() ? item->sample(selectedPoints[i]).y() : 0 ) ;

		// do coefficient calculation
		QList<double> coeffs = gaussjinv(matrix,yVals) ;
	}

	double scale = scaleAction->isChecked() ? coeffs.takeFirst() : 1. ,
	       offset= offsetAction->isChecked()? coeffs.takeFirst() : 0. ,
	       offline=offlineAction->isChecked()?coeffs.takeFirst() : 0. ;

	specPlotMoveCommand *command = new specPlotMoveCommand ;
	command->setItem(model->index(item)) ;
	command->setCorrections(shift,offset,offline,scale) ;
	undoPartner->push(command) ;
}
