#include "specspectrumplot.h"
#include "actionlib/specplotmovecommand.h"
#include "actionlib/specmulticommand.h"
#include "utility-functions.h"
#include <QBuffer>
#include "qwt/qwt_plot_renderer.h"
#include <qwt_scale_div.h>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <qwt_scale_draw.h>
#include "actionlib/specprintplotaction.h"
#include "specsvgitem.h"
#include "actionlib/specresizesvgcommand.h"
#include "actionlib/specexchangedatacommand.h"

void specSpectrumPlot::toggleAligning(bool on)
{
	foreach(QAction *action, alignmentActions->actions())
		action->setEnabled(on) ;
	alignWithReferenceAction->setEnabled(true) ;
	if (on)
	{
		alignmentPicker= new CanvasPicker(this) ;
		alignmentPicker->setOwning() ;
		connect(alignmentPicker, SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SLOT(applyZeroRanges(specCanvasItem*,int,double,double))) ;
	}
	else
		delete alignmentPicker ;
}

void specSpectrumPlot::invalidateReference()
{
	if (reference) delete reference ;
	setReferenceAction->setToolTip(QString("Momentan keine Referenz."));
}

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
	specPlot(parent),
	view(0),
	correctionPicker(0),
	alignmentPicker(0),
	reference(0),
	SVGpicker(0)
{
	correctionActions = new QActionGroup(this) ;
	correctionActions->setExclusive(false);
	offsetAction  = new QAction(QIcon(":/offset.png"), "Offset", correctionActions) ;
	offlineAction = new QAction(QIcon(":/offline.png"), "Slope",correctionActions) ;
	scaleAction   = new QAction(QIcon(":/scale.png"),"Scale", correctionActions) ;
	shiftAction   = new QAction("shift x",correctionActions) ;
	printAction   = new specPrintPlotAction(this) ;
	modifySVGs    = new QAction("Modify SVGs",this) ;
	subInterpolatedAction = new QAction(QIcon(":/multiminus.png"), "Subtraction",this) ;

	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;
	correctionActions->addAction(shiftAction) ;

	connect(correctionActions,SIGNAL(triggered(QAction*)),this,SLOT(correctionsChanged())) ;
	foreach(QAction *action, correctionActions->actions())
		action->setCheckable(true) ;
	modifySVGs->setCheckable(true) ;

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
	toggleAligning(false) ;


	QImage image(":/add.png") ;
	QByteArray byteArray ;
	QBuffer buffer(&byteArray) ;
	buffer.open(QIODevice::WriteOnly) ;
	image.save(&buffer,"PNG") ;

	invalidateReference();
//	alignmentActions->addAction(setReferenceAction) ;
	alignmentActions->addAction(alignWithReferenceAction) ;
	alignmentActions->addAction(addRangeAction) ;
	alignmentActions->addAction(removeRangeAction) ;
	alignmentActions->addAction(noSlopeAction) ;

	connect(alignWithReferenceAction,SIGNAL(toggled(bool)),correctionActions,SLOT(setDisabled(bool))) ;
	connect(alignWithReferenceAction,SIGNAL(toggled(bool)),this,SLOT(correctionsChanged())) ;
	connect(alignmentActions,SIGNAL(triggered(QAction*)),this,SLOT(alignmentChanged(QAction*))) ;
	connect(modifySVGs,SIGNAL(triggered(bool)),this,SLOT(modifyingSVGs(bool))) ;
	connect(subInterpolatedAction, SIGNAL(triggered()), this, SLOT(multipleSubtraction())) ;
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
	qDebug() << "VIEW:" << view ;
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
			invalidateReference();
			return ;
		}
		reference = new specDataItem(QVector<specDataPoint>(),QHash<QString,specDescriptor>()) ;
		qDebug() << "new reference size:" << reference->dataSize() << referenceDataItems.size() << referenceItems.size() ;
		for (int i = 0 ; i < referenceDataItems.size() ; ++i)
			reference->operator +=(*(referenceDataItems[i])) ;
		qDebug() << "new reference size (added):" << reference->dataSize() ;
		reference->flatten();
		reference->revalidate();
		reference->setPen(QPen(Qt::red));
		qDebug() << "new reference size (flattened):" << reference->dataSize() ;

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

		QByteArray byteArray ;
		QBuffer buffer(&byteArray) ;
		buffer.open(QIODevice::WriteOnly) ;
		plotImage.save(&buffer,"PNG") ;

		setReferenceAction->setToolTip(QString("Momentane Referenz:<br><img src=\"data:image/png;base64,%1\"></img>").arg(QString(buffer.data().toBase64())));

	}
	else if (action == alignWithReferenceAction)
	{
		toggleAligning(alignWithReferenceAction->isChecked());
	}
	else if (action == addRangeAction)
	{
		double min = axisScaleDiv(QwtPlot::xBottom)->lowerBound(), max = axisScaleDiv(QwtPlot::xBottom)->upperBound() ;
		specRange *newRange = new specRange(min+.1*(max-min),max-.1*(max-min)) ;
		newRange->attach(this) ;
		alignmentPicker->addSelectable(newRange) ;
	}
	else if (action == removeRangeAction)
		alignmentPicker->removeSelected();
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
	if (correctionChecked() && correctionActions->isEnabled())
	{
		if (!correctionPicker)
		{
			qDebug("installing picker") ;
			correctionPicker = new CanvasPicker(this) ;
			connect(correctionPicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this,SLOT(pointMoved(specCanvasItem*,int,double,double))) ;
			QList<specCanvasItem*> items ;
			QwtPlotItemList onPlot = itemList(spec::spectrum) ;
			foreach(QwtPlotItem* item, onPlot)
				items << dynamic_cast<specDataItem*>(item) ;
			items.removeAll(0) ;
			correctionPicker->addSelectable(items);
		}
	}
	else
	{
		delete correctionPicker ;
		correctionPicker = 0  ;
		pointHash.clear();
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


	double scale = 1, offset = 0, offline = 0 ;
	qDebug("computing coeffs") ;
	if (scaleAction->isChecked() || offsetAction->isChecked() || offlineAction->isChecked())
	{
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

		QList<double> coeffs ;
		QList<double> yVals ;
		yVals << y- (scaleAction->isChecked() ? 0. : item->sample(no).y()) ; // TODO verify!
		for (int i = 1 ; i < selectedPoints.size() && i < matrix.size() ; i++)
			yVals << (scaleAction->isChecked() ? item->sample(selectedPoints[i]).y() : 0 ) ;

		// do coefficient calculation
		coeffs = gaussjinv(matrix,yVals) ;

		scale = scaleAction->isChecked() && !coeffs.isEmpty() ? coeffs.takeFirst() : 1. ,
		offset= offsetAction->isChecked()&& !coeffs.isEmpty() ? coeffs.takeFirst() : 0. ,
		offline=offlineAction->isChecked()&&!coeffs.isEmpty() ?coeffs.takeFirst() : 0. ;
	}
	qDebug("pushing new move command %f %f %f %f",shift,offset,offline,scale) ;
	specPlotMoveCommand *command = new specPlotMoveCommand ;
	command->setItem(view->model()->index( (specModelItem*) item)) ; // TODO do dynamic cast first!!
	command->setCorrections(shift,offset,offline,scale) ;
	command->setParentWidget(view) ;
	undoPartner->push(command) ;
}

specMultiCommand * specSpectrumPlot::generateCorrectionCommand(
	const QwtPlotItemList& zeroRanges,
	const QwtPlotItemList& spectra,
	const QMap<double, double>& referenceSpectrum,
	const specView* view,
	bool noSlope)
{
	specMultiCommand *zeroCommand = new specMultiCommand ;
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
		// apply reference
		qDebug() << pointsInRange ;
		if (referenceSpectrum.size() > 1 && !pointsInRange.empty()) // TODO general return condition if pointsInRange is empty
		{
			// try to find two bordering points for linear interpolation for each point in range.
			for (QList<QPointF>::iterator i = pointsInRange.begin() ; i != pointsInRange.end() ; ++i)
			{
				double x = i->x() ;
				// if exact same x value is contained in reference, just take it.
				if (referenceSpectrum.contains(x))
				{
					i->setY(i->y()-referenceSpectrum[x]) ;
					continue ;
				}
				QMap<double,double>::const_iterator pointAfter = referenceSpectrum.upperBound(x);
				// no points for lin interpol found -> take point from correction list
				if (pointAfter == referenceSpectrum.begin() || pointAfter == referenceSpectrum.end())
				{
					pointsInRange.erase(i) ;
					continue ;
				}
				QMap<double,double>::const_iterator pointBefore  = pointAfter - 1;
				// subtract linear interpolation
				double x1 = pointBefore.key(),
						x2 = pointAfter.key(),
						y1 = pointBefore.value(),
						y2 = pointAfter.value(),
						y = i->y() ;
				i->setY(i->y()
					- pointBefore.value()
					- (pointAfter.value() - pointBefore.value()) /
					  (pointAfter.key()   - pointBefore.key()  ) *
					  (x - pointBefore.key())) ;
			}
			qDebug() << referenceSpectrum << pointsInRange ;
		}

		// compute correction
		double offset = 0, slope = 0 ;
		if((noSlope && !pointsInRange.isEmpty() )|| pointsInRange.size() == 1)
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
			qDebug() << "Matrix: " << matrix << "Vektor: " << vector ;
		}
		specPlotMoveCommand *command = new specPlotMoveCommand(zeroCommand) ;
		if (view && view->model())
			command->setItem(view->model()->index(spectrum));
		qDebug() << "setting slope and offset" << offset << slope ;
		command->setCorrections(0,-offset,-slope,1.) ;
	}
	return zeroCommand ;
}

void specSpectrumPlot::applyZeroRanges(specCanvasItem* range,int point, double newX, double newY)
{
	((specRange*) range)->pointMoved(point,newX,newY) ;
	QwtPlotItemList zeroRanges = itemList(spec::zeroRange) ;
	QwtPlotItemList spectra = itemList(spec::spectrum) ; // TODO roll back previous undo command if it was of the same kind and merge with what is to come.
	// prepare map of x and y values
	QMap<double,double> referenceSpectrum ;
	for (int i = 0 ; i < reference->dataSize() ; ++i)
		referenceSpectrum[reference->sample(i).x()] = reference->sample(i).y() ;

	specMultiCommand *zeroCommand = generateCorrectionCommand(zeroRanges, spectra, referenceSpectrum, view, noSlopeAction->isChecked()) ;
	zeroCommand->setParentWidget(view) ;
	undoPartner->push(zeroCommand) ;
	replot() ;
}

void specSpectrumPlot::modifyingSVGs(const bool &modify)
{
	if (SVGpicker)
	{
		disconnect(SVGpicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)),this,SLOT(resizeSVG(specCanvasItem*,int,double,double))) ;
		delete SVGpicker ;
		SVGpicker = 0 ;
	}
	if (!modify) return ;

	SVGpicker = new CanvasPicker(this) ;
	QList<specCanvasItem*> SVGitems ;

	foreach(QwtPlotItem *item, itemList())
		SVGitems << dynamic_cast<specSVGItem*>(item) ;
	SVGitems.removeAll(0) ;
	SVGpicker->addSelectable(SVGitems) ;
	connect(SVGpicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)),this,SLOT(resizeSVG(specCanvasItem*,int,double,double))) ;
}

void specSpectrumPlot::resizeSVG(specCanvasItem *item, int point, double x, double y)
{
	specSVGItem *pointer = dynamic_cast<specSVGItem*>(item) ;
	if (!pointer || !view || !undoPartner) return ;
	QRectF bounds = pointer->boundingRect() ;
	double aspectRatio = bounds.width()/bounds.height() ;
	QPoint newPoint(x,y) ;

	switch (point)
	{
	case specSVGItem::center :
		bounds.moveCenter(QPoint(x,y)); ; break ;
	case specSVGItem::left :
		bounds.setLeft(x) ; break ;
	case specSVGItem::right :
		bounds.setRight(x) ; break ;
	case specSVGItem::top :
		bounds.setTop(y) ; break ;
	case specSVGItem::bottom :
		bounds.setBottom(y) ; break ;
	case specSVGItem::topLeft :
		bounds.setTopLeft(newPoint) ; break ;
	case specSVGItem::topRight :
		bounds.setTopRight(newPoint) ; break ;
	case specSVGItem::bottomLeft :
		bounds.setBottomLeft(newPoint) ; break ;
	case specSVGItem::bottomRight :
		bounds.setBottomRight(newPoint); break ;
	}
	bounds = bounds.normalized() ;
	double newAspect = bounds.width()/bounds.height() ;
	if (newAspect < aspectRatio && (point == specSVGItem::topLeft || point == specSVGItem::bottomLeft))
		bounds.setLeft(bounds.right()-bounds.height()*aspectRatio) ;
	else if (newAspect < aspectRatio && (point == specSVGItem::topRight || point == specSVGItem::bottomRight))
		bounds.setRight(bounds.left()+bounds.height()*aspectRatio) ;
	else if (newAspect > aspectRatio && (point == specSVGItem::topRight || point == specSVGItem::topLeft))
		bounds.setTop(bounds.bottom()-bounds.width()/aspectRatio) ;
	else if (newAspect > aspectRatio && (point == specSVGItem::bottomLeft || point == specSVGItem::bottomRight))
		bounds.setBottom(bounds.top()+bounds.width()/aspectRatio) ;

	specResizeSVGcommand *command = new specResizeSVGcommand ;
	command->setParentWidget(this) ;
	command->setItem(view->model()->index(pointer),bounds) ;
	undoPartner->push(command) ;
}

void specSpectrumPlot::multipleSubtraction()
{
	QwtPlotItemList spectra = itemList(spec::spectrum) ;
	// prepare map of x and y values
	QMap<double,double> referenceSpectrum ; // TODO convert reference spectrum to map!
	for (int i = 0 ; i < reference->dataSize() ; ++i)
		referenceSpectrum[reference->sample(i).x()] = reference->sample(i).y() ;
	// TODO more efficient undo:  generate added/subtracted spectrum (interpolated points included)
	specMultiCommand* subCommand = new specMultiCommand ;
	for (int i = 0 ; i < spectra.size() ; ++i)
	{
		specDataItem* spectrum = dynamic_cast<specDataItem*>(spectra[i]) ;
		if (!spectrum) continue ;
		QVector<specDataPoint> data = spectrum->allData() ;
		spectrum->applyCorrection(data) ;
		QMap<double,double>::iterator lower, upper ;
		for (int i = 0 ; i < data.size() ; ++i)
		{
			lower = referenceSpectrum.lowerBound(data[i].nu) ;
			if (lower == referenceSpectrum.end()) continue ;
			double toSub = 0 ;
			if (lower.key() == data[i].nu)
				toSub = lower.value() ;
			else // start interpolating
			{
				if (lower == referenceSpectrum.begin()) continue ;
				upper = lower-- ;
				toSub = lower.value() + (upper.value()-lower.value())
							 * (data[i].nu-lower.key())
							 / (upper.key()-lower.key());
			}
			data[i].sig -= toSub ;
		}
		spectrum->reverseCorrection(data) ;
		specExchangeDataCommand *command = new specExchangeDataCommand(subCommand) ;
		if (view && view->model())
			command->setItem(view->model()->index(spectrum),data);
	}

	subCommand->setParentWidget(view) ;
	undoPartner->push(subCommand) ;
	replot() ;
}
