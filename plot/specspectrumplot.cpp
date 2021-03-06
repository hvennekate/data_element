unsigned int qHash(const double& d)
{
	return * ((unsigned int*)(&d)) ;
}
#include "specspectrumplot.h"
#include <QActionGroup>
#include <QBuffer>
#include <qwt_scale_draw.h>
#include "specmulticommand.h"
#include "specrange.h"
#include "specdataitem.h"
#include "canvaspicker.h"
#include "specfolderitem.h"
#include "specview.h"
#include "qwt_plot_renderer.h"
#include "utility-functions.h"
#include "specexchangefiltercommand.h"
#include "specactionlibrary.h"
#include "specexchangedatacommand.h"
#include <QMessageBox>
#include "specfiltergenerator.h"

specSpectrumPlot::moveMode specSpectrumPlot::correctionsStatus() const
{
	qDebug() << ((offsetAction->isChecked() ? Offset : NoMoveMode)
		     | (offlineAction->isChecked() ? Slope : NoMoveMode)
		     | (scaleAction->isChecked() ? Scale : NoMoveMode)) ;
	qDebug() << (offsetAction->isChecked() ? Offset : NoMoveMode)
		 << (offlineAction->isChecked() ? Slope : NoMoveMode)
		 << (scaleAction->isChecked() ? Scale : NoMoveMode) ;
	return (offsetAction->isChecked() ? Offset : NoMoveMode)
	       | (offlineAction->isChecked() ? Slope : NoMoveMode)
	       | (scaleAction->isChecked() ? Scale : NoMoveMode) ;
}

void specSpectrumPlot::setCorrectionsStatus(moveMode m)
{
	offsetAction->setChecked(m & Offset) ;
	offlineAction->setChecked(m & Slope) ;
	scaleAction->setChecked(m & Scale) ;
}

void specSpectrumPlot::checkReferenceForScaling()
{
	if(alignWithReferenceAction->isChecked() && !reference)
	{
		scaleAction->setChecked(false) ;
		scaleAction->setEnabled(false) ;
	}
	else
		scaleAction->setEnabled(true) ;
}

void specSpectrumPlot::toggleAligning(bool on)
{
	alignmentActions->setEnabled(on) ;
	if(on)
	{
		alignmentPicker = new CanvasPicker(this) ;
		alignmentPicker->setOwning() ;
		connect(alignmentPicker, SIGNAL(pointMoved(specCanvasItem*, int, double, double)), this, SLOT(applyZeroRanges(specCanvasItem*, int, double, double))) ;
		manualAlignment = correctionsStatus() ;
		setCorrectionsStatus(rangeAlignment) ;
		removeRangeAction->setEnabled(false) ;
		applyRangesAction->setEnabled(false) ;
	}
	else
	{
		delete alignmentPicker ;
		alignmentPicker = 0 ;
		rangeAlignment = correctionsStatus() ;
		setCorrectionsStatus(manualAlignment);
	}
	alignWithReferenceAction->setEnabled(true) ;
}

void specSpectrumPlot::invalidateReference()
{
	if(reference) delete reference ;
	reference = 0 ;
	setReferenceAction->setToolTip(QString("Currently no reference to show."));
	subInterpolatedAction->setDisabled(true);
}

specSpectrumPlot::specSpectrumPlot(QWidget* parent) :
	specPlot(parent),
	correctionPicker(0),
	alignmentPicker(0),
	manualAlignment(NoMoveMode),
	rangeAlignment(NoMoveMode),
	reference(0)
{
	correctionActions = new QActionGroup(this) ;
	alignmentActions = new QActionGroup(this) ;
	alignWithReferenceAction = new QAction(QIcon(":/zeroCorrection.png"), tr("Align"), this) ;

	offsetAction  = new QAction(QIcon(":/offset.png"), tr("Offset"), correctionActions) ;
	offlineAction = new QAction(QIcon(":/offline.png"), tr("Slope"), correctionActions) ;
	scaleAction   = new QAction(QIcon(":/scale.png"), tr("Scale"), correctionActions) ;
	shiftAction   = new QAction(QIcon(":/xshift.png"), tr("Shift x"), correctionActions) ;

	alignWithReferenceAction->setCheckable(true) ;
	alignWithReferenceAction->setChecked(false) ;
	addRangeAction = new QAction(QIcon(":/addZeroRange.png"), tr("Add aligning range"), alignmentActions) ;
	removeRangeAction = new QAction(QIcon(":/deleteZeroRange.png"), tr("Delete aligning range"), alignmentActions) ;
	applyRangesAction = new QAction(QIcon(":/zeroRange.png"), tr("Apply current zero ranges"), alignmentActions) ;
	applyRangesAction->setShortcut(tr("a"));

	setReferenceAction = new QAction(QIcon(":/data.png"), tr("Set reference"), this) ;
	setReferenceAction->setShortcut(tr("r"));
	subInterpolatedAction = new QAction(QIcon(":/multiminus.png"), tr("Subtract Reference"), this) ;
	subInterpolatedAction->setShortcut(tr("s"));

	correctionActions->setExclusive(false);
	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;
	correctionActions->addAction(shiftAction) ;
	foreach(QAction * action, correctionActions->actions())
	action->setCheckable(true) ;

	alignmentActions->setExclusive(false) ;
	//	alignmentActions->addAction(alignWithReferenceAction) ;
	alignmentActions->addAction(addRangeAction) ;
	alignmentActions->addAction(removeRangeAction) ;
	alignmentActions->addAction(applyRangesAction) ;

	connect(correctionActions, SIGNAL(triggered(QAction*)), this, SLOT(correctionsChanged(QAction*))) ;
	connect(alignWithReferenceAction, SIGNAL(toggled(bool)), shiftAction, SLOT(setDisabled(bool))) ;
	//	connect(alignWithReferenceAction,SIGNAL(toggled(bool)), this, SLOT(correctionsChanged())) ;
	connect(alignWithReferenceAction, SIGNAL(toggled(bool)), this, SLOT(toggleAligning(bool))) ;
	connect(alignmentActions, SIGNAL(triggered(QAction*)), this, SLOT(alignmentChanged(QAction*))) ;
	connect(subInterpolatedAction, SIGNAL(triggered()), this, SLOT(multipleSubtraction())) ;
	connect(setReferenceAction, SIGNAL(triggered()), this, SLOT(setReference())) ;
	connect(alignWithReferenceAction, SIGNAL(toggled(bool)), this, SLOT(checkReferenceForScaling())) ;
	connect(setReferenceAction, SIGNAL(triggered()), this, SLOT(checkReferenceForScaling())) ;
	connect(applyRangesAction, SIGNAL(triggered()), this, SLOT(applyZeroRanges())) ;

	toggleAligning(false) ;
	rangeAlignment = (Offset | Slope) ;
	invalidateReference();

	offsetAction->setWhatsThis(tr("When this button is enabled, you can correct your data for any offset by individually moving any point of a data curve to where it ought to be."));
	offlineAction->setWhatsThis(tr("By enabling this button, you enable detrending correction.  Pick any point of a data curve and move it to subtract a trend."));
	scaleAction->setWhatsThis(tr("This button lets you scale data by moving a data point."));
	shiftAction->setWhatsThis(tr("This button enables shifting data along the x axis by moving a data point."));
	subInterpolatedAction->setWhatsThis(tr("Subtract the reference data from selected data sets (applying interpolation if necessary)."));
	setReferenceAction->setWhatsThis(tr("Sets the reference for interpolated subtractions and/or for aligning other data sets."));
	alignWithReferenceAction->setWhatsThis(tr("Enables aligning selected data sets with the reference defined (or with the x axis if no reference has been set).\nFor aligning, a linear function will be subtracted, unless the correction has been limited to subtracting an offset."));
	addRangeAction->setWhatsThis(tr("Add a range for alignment.  The alignment correction of data sets selected will be calculated based on the points that lie within at least one of those ranges.")) ;
	removeRangeAction->setWhatsThis(tr("Delete a range for alignment.  The alignment correction of data sets selected will be calculated based on the points that lie within at least one of those ranges."));
	applyRangesAction->setWhatsThis(tr("Applies the currently active zero ranges")) ;

	setObjectName("mainPlot");
}

QList<specDataItem*> specSpectrumPlot::folderContent(specModelItem* folder)
{
	QList<specDataItem*> content ;
	if(dynamic_cast<specDataItem*>(folder))
		content << (specDataItem*) folder ;
	for(int i = 0 ; i < folder->children() ; ++i)
	{
		specModelItem* item = ((specFolderItem*) folder)->child(i) ;
		if(dynamic_cast<specDataItem*>(item))
			content << (specDataItem*) item ;
		else if(item->isFolder())
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
	if(referenceDataItems.isEmpty())
		return ;
	reference = new specDataItem(QVector<specDataPoint>(), QHash<QString, specDescriptor>()) ;
	for(int i = 0 ; i < referenceDataItems.size() ; ++i)
		reference->operator += (* (referenceDataItems[i])) ;
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
	toolTipPlot.setAxisFont(QwtPlot::xBottom, font);
	toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setSpacing(2) ;
	toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setTickLength(QwtScaleDiv::MajorTick, 4) ;
	toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setTickLength(QwtScaleDiv::MediumTick, 3) ;
	toolTipPlot.axisScaleDraw(QwtPlot::xBottom)->setTickLength(QwtScaleDiv::MinorTick, 2) ;

	font = axisFont(QwtPlot::yLeft) ;
	font.setPixelSize(8) ;
	toolTipPlot.setAxisFont(QwtPlot::yLeft, font) ;
	toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setSpacing(2) ;
	toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setTickLength(QwtScaleDiv::MajorTick, 4) ;
	toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setTickLength(QwtScaleDiv::MediumTick, 3) ;
	toolTipPlot.axisScaleDraw(QwtPlot::yLeft)->setTickLength(QwtScaleDiv::MinorTick, 2) ;

	toolTipPlot.replot();

	QImage plotImage(200, 100, QImage::Format_ARGB32) ;
	plotImage.fill(0);

	QwtPlotRenderer renderer ;
	renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
	renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground, true) ;
	renderer.setLayoutFlags(QwtPlotRenderer::DefaultLayout);
	renderer.renderTo(&toolTipPlot, plotImage) ;

	QByteArray byteArray ; // TODO extra function (c.f. svgItem)
	QBuffer buffer(&byteArray) ;
	buffer.open(QIODevice::WriteOnly) ;
	plotImage.save(&buffer, "PNG") ;

	setReferenceAction->setToolTip(QString("Momentane Referenz:<br><img src=\"data:image/png;base64,%1\"></img>").arg(QString(buffer.data().toBase64())));
	subInterpolatedAction->setEnabled(true) ;
}

void specSpectrumPlot::alignmentChanged(QAction* action)
{
	if(action == addRangeAction)
	{
		double min = axisScaleDiv(QwtPlot::xBottom).lowerBound(), max = axisScaleDiv(QwtPlot::xBottom).upperBound() ;
		specRange* newRange = new specRange(min + .1 * (max - min), max - .1 * (max - min),
						    (axisScaleDiv(QwtPlot::yLeft).lowerBound() +
						     axisScaleDiv(QwtPlot::yLeft).upperBound()) / 2.) ;
		newRange->attach(this) ;
		alignmentPicker->addSelectable(newRange) ;
	}
	else if(action == removeRangeAction)
		alignmentPicker->removeSelected();
	bool enableRemove = alignmentPicker && alignmentPicker->countSelectable() ;
	removeRangeAction->setEnabled(enableRemove);
	applyRangesAction->setEnabled(enableRemove) ;
}

bool specSpectrumPlot::correctionChecked()
{
	bool checked = false ;
	foreach(QAction * action, correctionActions->actions())
	checked = checked || action->isChecked() ;
	return checked ;
}

void specSpectrumPlot::correctionsChanged(QAction* action)
{
	if(alignWithReferenceAction->isChecked() && !offsetAction->isChecked() && !offlineAction->isChecked())
	{
		if(action == offlineAction) offlineAction->setChecked(true) ;
		else offsetAction->setChecked(true) ;
	}
	if(correctionChecked() && !alignWithReferenceAction->isChecked())
	{
		if(!correctionPicker)
		{
			correctionPicker = new CanvasPicker(this) ;
			connect(correctionPicker, SIGNAL(pointMoved(specCanvasItem*, int, double, double)), this, SLOT(pointMoved(specCanvasItem*, int, double, double))) ;
			QList<specCanvasItem*> items ;
			QwtPlotItemList onPlot = itemList(spec::spectrum) ;
			foreach(QwtPlotItem * item, onPlot)
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
	if(correctionPicker)
		correctionPicker->addSelectable(item) ;
}

void specSpectrumPlot::detachFromPicker(specCanvasItem* item)
{
	if(correctionPicker)
		correctionPicker->removeSelectable(item) ;
}

void specSpectrumPlot::pointMoved(specCanvasItem* item, int no, double x, double y)
{
	if(!view || !view->model()) return ;
	specModelItem* modelItem = dynamic_cast<specModelItem*>(item) ;
	if(!modelItem) return ;
	// get reference to point list from point hash (in order to re-use old code below)
	QList<int>& selectedPoints = pointHash[item] ;
	// Add new Point to list.
	selectedPoints.prepend(selectedPoints.contains(no) ? selectedPoints.takeAt(selectedPoints.indexOf(no)) : no) ;

	// TODO take care of the case where there is no undo stack available...
	if(!undoPartner()) return ;

	// collect correction parameters
	QString failedCorrections ;
#define CHECKCORRECTIONPARAMETER(PARAMETER,SPECIALCONDITION,PHRASE,DEFAULT,ACTION) \
	if(!isfinite(PARAMETER) SPECIALCONDITION) { \
		failedCorrections += tr(PHRASE) + ":\t" + QString::number(PARAMETER) + "\n"; \
		PARAMETER = DEFAULT ; \
		ACTION->setChecked(false) ; }
	// x shift:
	double shift = shiftAction->isChecked() ? x - item->sample(no).x() : 0 ;
	CHECKCORRECTIONPARAMETER(shift, , "x shift", 0., shiftAction)

	// compute other corrections:
	// TODO rewrite code for GSL

	double scale = 1, offset = 0, offline = 0 ;
	if(scaleAction->isChecked() || offsetAction->isChecked() || offlineAction->isChecked())
	{
		QVector<QVector<double> > matrix ;
		for(int i = 0 ; i < selectedPoints.size() ; i++) matrix << QVector<double>() ;
		if(scaleAction->isChecked())
			for(int i = 0 ; i < selectedPoints.size(); i++)
				matrix[i] << item->sample(selectedPoints[i]).y() ;
		if(offsetAction->isChecked())
			for(int i = 0 ; i < selectedPoints.size(); i++)
				matrix[i] << 1. ;
		if(offlineAction->isChecked())
			for(int i = 0 ; i < selectedPoints.size(); i++)
				matrix[i] << item->sample(selectedPoints[i]).x() + shift ;
		for(int i = 0 ; i < matrix.size() ; i++)  // verringere, wenn noetig, die Spaltenzahl
			while(matrix.size() < matrix[i].size())
				matrix[i].resize(matrix[i].size() - 1) ;
		while(matrix[0].size() < matrix.size())  // verringere, wenn noetig, die Zeilenzahl
			matrix.resize(matrix.size() - 1) ;

		QVector<double> coeffs ;
		QVector<double> yVals ;
		yVals << y - (scaleAction->isChecked() ? 0. : item->sample(no).y()) ;  // TODO verify!
		for(int i = 1 ; i < selectedPoints.size() && i < matrix.size() ; i++)
			yVals << (scaleAction->isChecked() ? item->sample(selectedPoints[i]).y() : 0) ;

		// do coefficient calculation
		coeffs = gaussjinv(matrix, yVals) ;

		int i = 0 ; // TODO make offset and offline supersede scale
		scale = scaleAction->isChecked() && i < coeffs.size() ? coeffs[i++] : 1. ;
		offset = offsetAction->isChecked() && i < coeffs.size() ? coeffs[i++] : 0. ;
		offline = offlineAction->isChecked() && i < coeffs.size() ? coeffs[i++] : 0. ;

		CHECKCORRECTIONPARAMETER(scale, || !isnormal(scale), "scaling", 1., scaleAction)
		CHECKCORRECTIONPARAMETER(offset, , "offset", 0, offsetAction)
		CHECKCORRECTIONPARAMETER(offline, , "slope", 0, offlineAction)
	}
	if(!failedCorrections.isEmpty())
		QMessageBox::critical(0, tr("Aligning error"),
				      tr("The following required alignment parameters were illegal:\n")
				      + failedCorrections
				      + tr("The parameters were disabled.")) ;
	specExchangeFilterCommand* command = new specExchangeFilterCommand ;
	command->setParentObject(view->model()) ;
	command->setItem(modelItem) ;
	command->setRelativeFilter(specDataPointFilter(offset, offline, scale, shift)) ;
	undoPartner()->push(command) ;
}

void specSpectrumPlot::applyZeroRanges(specCanvasItem* range, int point, double newX, double newY)
{
	((specRange*) range)->pointMoved(point, newX, newY) ;
	applyZeroRanges();
}

void specSpectrumPlot::applyZeroRanges()
{
	QwtPlotItemList zeroRanges = itemList(spec::zeroRange) ;
	if(zeroRanges.isEmpty()) return ;
	QwtPlotItemList spectra = itemList(spec::spectrum) ;  // TODO roll back previous undo command if it was of the same kind and merge with what is to come.
	// REMARK:  should be implemented already.
	if(spectra.isEmpty()) return ;

	specFilterGenerator filterGenerator ;
	if (reference)
		filterGenerator.setReference(reference->dataMap()) ;
	filterGenerator.calcOffset(offsetAction->isChecked());
	filterGenerator.calcScale(scaleAction->isChecked()) ;
	filterGenerator.calcSlope(offlineAction->isChecked()) ;
	filterGenerator.setRanges(zeroRanges);

	specMultiCommand* zeroCommand = new specMultiCommand ;
	zeroCommand->setParentObject(view->model()) ;
	foreach(QwtPlotItem* s, spectra)
	{
		specModelItem* spectrum = dynamic_cast<specModelItem*>(s) ;
		if (!spectrum) continue ;
		specDataPointFilter filter = filterGenerator.generateFilter(spectrum) ;
		if (!filter.valid()) continue ;
		specExchangeFilterCommand* command = new specExchangeFilterCommand(zeroCommand) ;
		command->setParentObject(view->model()) ;
		command->setItem(spectrum) ;
		command->setRelativeFilter(filter) ;
	}

	QStringList rangeStrings ;
	foreach(QwtPlotItem* i, zeroRanges)
	{
		specRange* range = (specRange*) i ;
		rangeStrings << QString::number(range->minValue()) + "--" + QString::number(range->maxValue()) ;
	}
	zeroCommand->setText(tr("Apply range correction. Ranges: ") + rangeStrings.join(", "));
	undoPartner()->push(zeroCommand) ;
	replot() ;
}

void specSpectrumPlot::multipleSubtraction()
{
	if(!reference) return ;
	QwtPlotItemList spectra = itemList(spec::spectrum) ;
	// prepare map of x and y values
	QMap<double, double> referenceSpectrum ; // TODO convert reference spectrum to map!
	for(size_t i = 0 ; i < reference->dataSize() ; ++i)
		referenceSpectrum[reference->sample(i).x()] = reference->sample(i).y() ;
	// TODO more efficient undo:  generate added/subtracted spectrum (interpolated points included)
	specMultiCommand* subCommand = new specMultiCommand ;
	if(view && view->model())
		subCommand->setParentObject(view->model());
	for(int i = 0 ; i < spectra.size() ; ++i)
	{
		specDataItem* spectrum = dynamic_cast<specDataItem*>(spectra[i]) ;
		if(!spectrum) continue ;
		QVector<specDataPoint> data = spectrum->allData() ;
		spectrum->applyCorrection(data) ;
		QMap<double, double>::iterator lower, upper ;
		for(int i = 0 ; i < data.size() ; ++i)
		{
			lower = referenceSpectrum.lowerBound(data[i].nu) ;
			if(lower == referenceSpectrum.end()) continue ;
			double toSub = 0 ;
			if(lower.key() == data[i].nu)
				toSub = lower.value() ;
			else // start interpolating
			{
				if(lower == referenceSpectrum.begin()) continue ;
				upper = lower-- ;
				toSub = lower.value() + (upper.value() - lower.value())
					* (data[i].nu - lower.key())
					/ (upper.key() - lower.key());
			}
			data[i].sig -= toSub ;
		}
		spectrum->reverseCorrection(data) ;
		specExchangeDataCommand* command = new specExchangeDataCommand(subCommand) ;
		if(view && view->model())
		{
			command->setParentObject(view->model());
			command->setItem(spectrum, data);
		}
	}

	subCommand->setText(tr("Subtract reference"));
	undoPartner()->push(subCommand) ;
	replot() ;
}

QList<QAction*> specSpectrumPlot::actions()
{
	return specPlot::actions() << correctionActions->actions()
	       << setReferenceAction
	       << alignWithReferenceAction
	       << alignmentActions->actions()
	       << subInterpolatedAction ;
}

specSpectrumPlot::~specSpectrumPlot()
{
	if(correctionPicker) correctionPicker->purgeSelectable();
	delete correctionPicker ;
	delete alignmentPicker ;
}
