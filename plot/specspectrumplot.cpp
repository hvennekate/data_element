#include "specspectrumplot.h"
#include "actionlib/specplotmovecommand.h"
#include "actionlib/specmultiplotmovecommand.h"
#include "utility-functions.h"
#include <QBuffer>
#include "qwt/qwt_plot_renderer.h"
#include <qwt_scale_div.h>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <qwt_scale_draw.h>

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
	specPlot(parent),
	view(0),
	picker(0),
	reference(0)
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


	QImage image(":/add.png") ;
	QByteArray byteArray ;
	QBuffer buffer(&byteArray) ;
	buffer.open(QIODevice::WriteOnly) ;
	image.save(&buffer,"PNG") ;

	setReferenceAction->setToolTip(QString("test<b>fett</b> <img src=\"data:image/png;base64,%1\"></img>").arg(QString(buffer.data().toBase64())));

	alignmentActions->addAction(setReferenceAction) ;
	alignmentActions->addAction(alignWithReferenceAction) ;
	alignmentActions->addAction(addRangeAction) ;
	alignmentActions->addAction(removeRangeAction) ;
	alignmentActions->addAction(noSlopeAction) ;


	connect(alignmentActions,SIGNAL(triggered(QAction*)),this,SLOT(alignmentChanged(QAction*))) ;
}

QList<specDataItem*> specSpectrumPlot::folderContent(specModelItem *folder)
{
	QList<specDataItem*> content ;
	if (dynamic_cast<specDataItem*>(folder))
		content << (specDataItem*) folder ;
	for (int i = 0 ; i < folder->children() ; ++i)
	{
		specModelItem* item = ((specFolderItem*)folder)->child(i) ;
		if (dynamic_cast<specDataItem*>(item))
			content << (specDataItem*) item ;
		else if (item->isFolder())
			content << folderContent((specFolderItem*) item) ;
	}
	return content ;
}

void specSpectrumPlot::alignmentChanged(QAction *action)
{
	if (action == setReferenceAction) // turn this into an undo command.
	{
		if (reference) delete reference ;
		QModelIndexList referenceItems = view->getSelection() ;

		// convert indexes to pointers
		QList<specDataItem*> referenceDataItems ;
		for(int i = 0 ; i < referenceItems.size() ; ++i)
			referenceDataItems << folderContent(view->model()->itemPointer(referenceItems[i])) ;
		if (referenceDataItems.isEmpty())
		{
			reference = 0 ;
			return ;
		}
		reference = new specDataItem(QList<specDataPoint>(),QHash<QString,specDescriptor>()) ;
		for (int i = 0 ; i < referenceDataItems.size() ; ++i)
			reference->operator +=(*(referenceDataItems[i])) ;
		reference->flatten();
		reference->setPen(QPen(Qt::red));

		// build tooltip
		QwtPlot toolTipPlot ;
		toolTipPlot.setAutoDelete(false) ;
		reference->attach(&toolTipPlot) ;
		toolTipPlot.replot();

		QFont font = axisFont(QwtPlot::xBottom) ;
		font.setPixelSize(8) ;
		toolTipPlot.setAxisFont(QwtPlot::xBottom,font);
		toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setSpacing(2) ;
		toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setTickLength(QwtScaleDiv::MajorTick,4) ;
		toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setTickLength(QwtScaleDiv::MediumTick,3) ;
		toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setTickLength(QwtScaleDiv::MinorTick,2) ;

		font = axisFont(QwtPlot::yLeft) ;
		qDebug() << "old size:" << font.pixelSize() ;
		font.setPixelSize(8) ;
		qDebug() << "new size:" << font.pixelSize() ;
		toolTipPlot.setAxisFont(QwtPlot::yLeft,font) ;
		toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setSpacing(2) ;
		toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setTickLength(QwtScaleDiv::MajorTick,4) ;
		toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setTickLength(QwtScaleDiv::MediumTick,3) ;
		toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setTickLength(QwtScaleDiv::MinorTick,2) ;

		toolTipPlot.replot();

		QImage plotImage(200,100,QImage::Format_ARGB32) ;
		plotImage.fill(0);

		QwtPlotRenderer renderer ;
		renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
		renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground,true) ;
		renderer.setLayoutFlag(QwtPlotRenderer::KeepFrames, false);
		renderer.renderTo(&toolTipPlot,plotImage) ;

		QDialog newDialog ;
		newDialog.setLayout(new QVBoxLayout) ;
		QLabel *newLabel =new QLabel(&newDialog) ;
		newLabel->setPixmap(QPixmap::fromImage(plotImage)) ;
		newDialog.layout()->addWidget(newLabel);
		newDialog.exec() ;

		QByteArray byteArray ;
		QBuffer buffer(&byteArray) ;
		buffer.open(QIODevice::WriteOnly) ;
		plotImage.save(&buffer,"PNG") ;

		setReferenceAction->setToolTip(QString("Momentane Referenz:<br><img src=\"data:image/png;base64,%1\"></img>").arg(QString(buffer.data().toBase64())));

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
	specMultiPlotMoveCommand *zeroCommand = new specMultiPlotMoveCommand ; // TODO establish extra class
	zeroCommand->setParentWidget(view) ;
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
