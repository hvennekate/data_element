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
}

void specSpectrumPlot::correctionsChanged()
{
	bool checked = false ;
	foreach(QAction *action, correctionActions->actions())
		checked = checked || action->isChecked() ;
	if (!checked)
	{
		// TODO unset highlighting
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
