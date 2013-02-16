#include "specspectrumplot.h"

#include <QActionGroup>
#include <QBuffer>
#include "qwt_scale_draw.h"
#include "specmulticommand.h"
#include "specrange.h"
#include "specdataitem.h"
#include "canvaspicker.h"
#include "specfolderitem.h"
#include "specview.h"
#include "qwt_plot_renderer.h"
#include "utility-functions.h"
#include "specplotmovecommand.h"
#include "specactionlibrary.h"
#include "specexchangedatacommand.h"

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
	{
		delete alignmentPicker ;
		alignmentPicker = 0 ;
	}
}

void specSpectrumPlot::invalidateReference()
{
	if (reference) delete reference ;
	reference = 0 ;
	setReferenceAction->setToolTip(QString("Momentan keine Referenz."));
	subInterpolatedAction->setDisabled(true);
}

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
	specPlot(parent),
	correctionPicker(0),
	alignmentPicker(0),
	reference(0)
{
	correctionActions = new QActionGroup(this) ;
	correctionActions->setExclusive(false);
	offsetAction  = new QAction(QIcon(":/offset.png"), tr("Offset"), correctionActions) ;
	offlineAction = new QAction(QIcon(":/offline.png"), tr("Slope"),correctionActions) ;
	scaleAction   = new QAction(QIcon(":/scale.png"), tr("Scale"), correctionActions) ;
	shiftAction   = new QAction(QIcon(":/xshift.png"), tr("Shift x"),correctionActions) ;
	subInterpolatedAction = new QAction(QIcon(":/multiminus.png"), tr("Subtract Reference"),this) ;

	subInterpolatedAction->setWhatsThis(tr("Subtract the reference data from selected data sets (applying interpolation if necessary)."));

	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;
	correctionActions->addAction(shiftAction) ;
	offsetAction->setWhatsThis(tr("When this button is enabled, you can correct your data for any offset by individually moving any point of a data curve to where it ought to be."));
	offlineAction->setWhatsThis(tr("By enabling this button, you enable detrending correction.  Pick any point of a data curve and move it to subtract a trend."));
	scaleAction->setWhatsThis(tr("This button lets you scale data by moving a data point."));
	shiftAction->setWhatsThis(tr("This button enables shifting data along the x axis by moving a data point."));


	connect(correctionActions,SIGNAL(triggered(QAction*)),this,SLOT(correctionsChanged())) ;
	foreach(QAction *action, correctionActions->actions())
		action->setCheckable(true) ;

	alignmentActions = new QActionGroup(this) ;
	alignmentActions->setExclusive(false) ;
	setReferenceAction = new QAction(QIcon(":/data.png"),tr("Set Reference"),this) ;
	alignWithReferenceAction = new QAction(QIcon(":/zeroCorrection.png"),tr("Align"),alignmentActions) ;
	alignWithReferenceAction->setCheckable(true) ;
	alignWithReferenceAction->setChecked(false) ;
	addRangeAction = new QAction(QIcon(":/addZeroRange.png"),tr("Add Aligning Range"),alignmentActions) ;
	removeRangeAction = new QAction(QIcon(":/deleteZeroRange.png"),tr("Delete Aligning Range"),alignmentActions) ;
	noSlopeAction = new QAction(QIcon(":/offset.png"),tr("Disable Slope"),alignmentActions) ;
	noSlopeAction->setCheckable(true) ;
	noSlopeAction->setChecked(false) ;
	toggleAligning(false) ;
	connect(setReferenceAction,SIGNAL(triggered()),this,SLOT(setReference())) ;

	setReferenceAction->setWhatsThis(tr("Sets the reference for interpolated subtractions and/or for aligning other data sets."));
	alignWithReferenceAction->setWhatsThis(tr("Enables aligning selected data sets with the reference defined (or with the x axis if no reference has been set).\nFor aligning, a linear function will be subtracted, unless the correction has been limited to subtracting an offset."));
	addRangeAction->setWhatsThis(tr("Add a range for alignment.  The alignment correction of data sets selected will be calculated based on the points that lie within at least one of those ranges.")) ;
	removeRangeAction->setWhatsThis(tr("Delete a range for alignment.  The alignment correction of data sets selected will be calculated based on the points that lie within at least one of those ranges."));
	noSlopeAction->setWhatsThis(tr("Use only an offset for aligning data sets with the reference data."));


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

void specSpectrumPlot::setReference()
{
	invalidateReference();
	QModelIndexList referenceItems = view->getSelection() ;

	// convert indexes to pointers
	QList<specDataItem*> referenceDataItems ;
	for(int i = 0 ; i < referenceItems.size() ; ++i)
		referenceDataItems << folderContent(view->model()->itemPointer(referenceItems[i])) ;
	if (referenceDataItems.isEmpty())
		return ;
	reference = new specDataItem(QVector<specDataPoint>(),QHash<QString,specDescriptor>()) ;
	for (int i = 0 ; i < referenceDataItems.size() ; ++i)
		reference->operator +=(*(referenceDataItems[i])) ;
	reference->flatten();
	reference->revalidate();
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
	font.setPixelSize(8) ;
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

    QByteArray byteArray ; // TODO extra function (c.f. svgItem)
	QBuffer buffer(&byteArray) ;
	buffer.open(QIODevice::WriteOnly) ;
	plotImage.save(&buffer,"PNG") ;

	setReferenceAction->setToolTip(QString("Momentane Referenz:<br><img src=\"data:image/png;base64,%1\"></img>").arg(QString(buffer.data().toBase64())));
	subInterpolatedAction->setEnabled(true) ;
}

void specSpectrumPlot::alignmentChanged(QAction *action)
{
	if (action == alignWithReferenceAction)
	{
		toggleAligning(alignWithReferenceAction->isChecked());
	}
	else if (action == addRangeAction)
	{
		double min = axisScaleDiv(QwtPlot::xBottom)->lowerBound(), max = axisScaleDiv(QwtPlot::xBottom)->upperBound() ;
        specRange *newRange = new specRange(min+.1*(max-min),max-.1*(max-min),
                                            (axisScaleDiv(QwtPlot::yLeft)->lowerBound()+
                                             axisScaleDiv(QwtPlot::yLeft)->upperBound())/2.) ;
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
			correctionPicker = new CanvasPicker(this) ;
			connect(correctionPicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this,SLOT(pointMoved(specCanvasItem*,int,double,double))) ;
			QList<specCanvasItem*> items ;
			QwtPlotItemList onPlot = itemList(spec::spectrum) ;
			foreach(QwtPlotItem* item, onPlot)
				items << dynamic_cast<specDataItem*>(item) ;
			items.removeAll(0) ;
			correctionPicker->addSelectable(items.toSet());
		}
	}
	else
	{
		delete correctionPicker ;
		correctionPicker = 0  ;
		pointHash.clear();
	}
}

void specSpectrumPlot::attachToPicker(specCanvasItem* item)
{
	if (correctionPicker)
		correctionPicker->addSelectable(item) ;
}

void specSpectrumPlot::detachFromPicker(specCanvasItem* item)
{
	if (correctionPicker)
		correctionPicker->removeSelectable(item) ;
}

void specSpectrumPlot::pointMoved(specCanvasItem *item, int no, double x, double y)
{
	if (!view) return ;
	// get reference to point list from point hash (in order to re-use old code below)
	QList<int>& selectedPoints = pointHash[item] ;
	// Add new Point to list.
	selectedPoints.prepend(selectedPoints.contains(no) ? selectedPoints.takeAt(selectedPoints.indexOf(no)) : no) ;

	// TODO take care of the case where there is no undo stack available...
	if (!undoPartner()) return ;

	// collect correction parameters
	// x shift:
	double shift = shiftAction->isChecked() ? x-item->sample(no).x() : 0 ;

	// compute other corrections:
	// TODO rewrite code for GSL


	double scale = 1, offset = 0, offline = 0 ;
	if (scaleAction->isChecked() || offsetAction->isChecked() || offlineAction->isChecked())
	{
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
	specPlotMoveCommand *command = new specPlotMoveCommand ;
	command->setItem(view->model()->index( (specModelItem*) item)) ; // TODO do dynamic cast first!!
	command->setCorrections(shift,offset,offline,scale) ;
	command->setParentObject(view->model()) ;
	undoPartner()->push(command) ;
}

specMultiCommand * specSpectrumPlot::generateCorrectionCommand(
	const QwtPlotItemList& zeroRanges,
	const QwtPlotItemList& spectra,
	const QMap<double, double>& referenceSpectrum,
	specModel* model,
	bool noSlope)
{
	specMultiCommand *zeroCommand = new specMultiCommand ;
	zeroCommand->setParentObject(model);
	for (int i = 0 ; i < spectra.size() ; ++i)
	{
		QList<QPointF> pointsInRange ;
		specModelItem* spectrum = (specModelItem*) spectra[i] ;
		// extract points that lie in any of the ranges
		for (size_t j = 0 ; j < spectrum->dataSize() ; ++j)
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
		if (referenceSpectrum.size() > 1 && !pointsInRange.empty()) // TODO general return condition if pointsInRange is empty
		{
			// try to find two bordering points for linear interpolation for each point in range.
            QList<QPointF>::iterator i = pointsInRange.begin() ;
            while (i != pointsInRange.end())
			{
                double x = i->x() ;
				// if exact same x value is contained in reference, just take it.
				if (referenceSpectrum.contains(x))
					i->setY(i->y()-referenceSpectrum[x]) ;
                else
                {
                    QMap<double,double>::const_iterator pointAfter = referenceSpectrum.upperBound(x);
                    // no points for lin interpol found -> take point from correction list
                    if (pointAfter == referenceSpectrum.begin() || // no point before to interpolate with
                            pointAfter == referenceSpectrum.end()) // no point after to interpolate with
                    {
                        i = pointsInRange.erase(i) ;
                        continue ;
                    }
                    QMap<double,double>::const_iterator pointBefore  = pointAfter - 1;
                    // subtract linear interpolation
                    i->setY(i->y()
                        - pointBefore.value()
                        - (pointAfter.value() - pointBefore.value()) /
                          (pointAfter.key()   - pointBefore.key()  ) *
                          (x - pointBefore.key())) ;
                }
                ++i ; // move on...
			}
		}

		// compute correction
		double offset = 0, slope = 0 ;
		if((noSlope && !pointsInRange.isEmpty() )|| pointsInRange.size() == 1)
		{
			// offset only
			for (int j = 0 ; j < pointsInRange.size() ; ++j)
				offset += pointsInRange[j].y() ;
			offset /= pointsInRange.size() ;
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
                if (isnan(y) || isnan(x)) continue ;
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
		if (model)
			command->setItem(model->index(spectrum));
		command->setCorrections(0,-offset,-slope,1.) ;
		command->setParentObject(model);
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
	if (reference)
		for (size_t i = 0 ; i < reference->dataSize() ; ++i)
			referenceSpectrum[reference->sample(i).x()] = reference->sample(i).y() ;

	specMultiCommand *zeroCommand = generateCorrectionCommand(zeroRanges, spectra, referenceSpectrum, view->model(), noSlopeAction->isChecked()) ;
    QStringList rangeStrings ;
    for (QwtPlotItemList::iterator i = zeroRanges.begin() ; i != zeroRanges.end() ; ++i)
    {
        specRange* range = (specRange*) (*i) ;
        rangeStrings << QString::number(range->minValue()) + "--" + QString::number(range->maxValue()) ;
    }
    zeroCommand->setText(tr("Apply range correction. Ranges: ") + rangeStrings.join(", "));
	undoPartner()->push(zeroCommand) ;
	replot() ;
}

void specSpectrumPlot::multipleSubtraction()
{
	if (!reference) return ;
	QwtPlotItemList spectra = itemList(spec::spectrum) ;
	// prepare map of x and y values
	QMap<double,double> referenceSpectrum ; // TODO convert reference spectrum to map!
	for (size_t i = 0 ; i < reference->dataSize() ; ++i)
		referenceSpectrum[reference->sample(i).x()] = reference->sample(i).y() ;
	// TODO more efficient undo:  generate added/subtracted spectrum (interpolated points included)
	specMultiCommand* subCommand = new specMultiCommand ;
	if (view && view->model())
		subCommand->setParentObject(view->model());
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
		{
			command->setParentObject(view->model());
			command->setItem(view->model()->index(spectrum),data);
		}
	}

	subCommand->setParentObject(view->model()) ;
	subCommand->setText(tr("Subtract reference"));
	undoPartner()->push(subCommand) ;
	replot() ;
}

QList<QAction*> specSpectrumPlot::actions()
{
	return specPlot::actions() << correctionActions->actions()
				   << setReferenceAction
				   << alignmentActions->actions()
				   << subInterpolatedAction ;
}

specSpectrumPlot::~specSpectrumPlot()
{
	if (correctionPicker) correctionPicker->purgeSelectable();
	delete correctionPicker ;
	delete alignmentPicker ;
}
