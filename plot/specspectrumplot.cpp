unsigned int qHash(const double& d)
{
	return *((unsigned int*) (&d)) ;
}
#include "specspectrumplot.h"
#include <QVector3D>
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
	if (alignWithReferenceAction->isChecked() && !reference)
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
	if (on)
	{
		alignmentPicker= new CanvasPicker(this) ;
		alignmentPicker->setOwning() ;
		connect(alignmentPicker, SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SLOT(applyZeroRanges(specCanvasItem*,int,double,double))) ;
		manualAlignment = correctionsStatus() ;
		setCorrectionsStatus(rangeAlignment) ;
		removeRangeAction->setEnabled(false) ;
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
	if (reference) delete reference ;
	reference = 0 ;
	setReferenceAction->setToolTip(QString("Momentan keine Referenz."));
	subInterpolatedAction->setDisabled(true);
}

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
	specPlot(parent),
	correctionPicker(0),
	alignmentPicker(0),
	manualAlignment(NoMoveMode),
	rangeAlignment(NoMoveMode),
	reference(0)
{
	correctionActions = new QActionGroup(this) ;
	alignmentActions = new QActionGroup(this) ;
	alignWithReferenceAction = new QAction(QIcon(":/zeroCorrection.png"),tr("Align"),this) ;

	offsetAction  = new QAction(QIcon(":/offset.png"), tr("Offset"), correctionActions) ;
	offlineAction = new QAction(QIcon(":/offline.png"), tr("Slope"),correctionActions) ;
	scaleAction   = new QAction(QIcon(":/scale.png"), tr("Scale"), correctionActions) ;
	shiftAction   = new QAction(QIcon(":/xshift.png"), tr("Shift x"),correctionActions) ;

	alignWithReferenceAction->setCheckable(true) ;
	alignWithReferenceAction->setChecked(false) ;
	addRangeAction = new QAction(QIcon(":/addZeroRange.png"),tr("Add aligning range"),alignmentActions) ;
	removeRangeAction = new QAction(QIcon(":/deleteZeroRange.png"),tr("Delete aligning range"),alignmentActions) ;

	setReferenceAction = new QAction(QIcon(":/data.png"),tr("Set reference"),this) ;
	subInterpolatedAction = new QAction(QIcon(":/multiminus.png"), tr("Subtract Reference"),this) ;

	correctionActions->setExclusive(false);
	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;
	correctionActions->addAction(shiftAction) ;
	foreach(QAction *action, correctionActions->actions())
		action->setCheckable(true) ;

	alignmentActions->setExclusive(false) ;
	//	alignmentActions->addAction(alignWithReferenceAction) ;
	alignmentActions->addAction(addRangeAction) ;
	alignmentActions->addAction(removeRangeAction) ;

	connect(correctionActions, SIGNAL(triggered(QAction*)), this, SLOT(correctionsChanged(QAction*))) ;
	connect(alignWithReferenceAction, SIGNAL(toggled(bool)), shiftAction, SLOT(setDisabled(bool))) ;
	//	connect(alignWithReferenceAction,SIGNAL(toggled(bool)), this, SLOT(correctionsChanged())) ;
	connect(alignWithReferenceAction, SIGNAL(toggled(bool)), this, SLOT(toggleAligning(bool))) ;
	connect(alignmentActions, SIGNAL(triggered(QAction*)), this, SLOT(alignmentChanged(QAction*))) ;
	connect(subInterpolatedAction, SIGNAL(triggered()), this, SLOT(multipleSubtraction())) ;
	connect(setReferenceAction,SIGNAL(triggered()),this,SLOT(setReference())) ;
	connect(alignWithReferenceAction, SIGNAL(toggled(bool)), this, SLOT(checkReferenceForScaling())) ;
	connect(setReferenceAction, SIGNAL(triggered()), this, SLOT(checkReferenceForScaling())) ;

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

	setObjectName("mainPlot");
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
	if (action == addRangeAction)
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
	removeRangeAction->setEnabled(alignmentPicker && alignmentPicker->countSelectable());
}

bool specSpectrumPlot::correctionChecked()
{
	bool checked = false ;
	foreach(QAction *action, correctionActions->actions())
		checked = checked || action->isChecked() ;
	return checked ;
}

void specSpectrumPlot::correctionsChanged(QAction* action)
{
	if (alignWithReferenceAction->isChecked() && !offsetAction->isChecked() && !offlineAction->isChecked())
	{
		if (action == offlineAction) offlineAction->setChecked(true) ;
		else offsetAction->setChecked(true) ;
	}
	if (correctionChecked() && !alignWithReferenceAction->isChecked())
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
	if (!view || !view->model()) return ;
	specModelItem* modelItem = dynamic_cast<specModelItem*>(item) ;
	if (!modelItem) return ;
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
		QVector<QVector<double> > matrix ;
		for (int i = 0 ; i < selectedPoints.size() ; i++) matrix << QVector<double>() ;
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
				matrix[i].resize(matrix[i].size()-1) ;
		while(matrix[0].size() < matrix.size()) // verringere, wenn noetig, die Zeilenzahl
			matrix.resize(matrix.size()-1) ;

		QVector<double> coeffs ;
		QVector<double> yVals ;
		yVals << y- (scaleAction->isChecked() ? 0. : item->sample(no).y()) ; // TODO verify!
		for (int i = 1 ; i < selectedPoints.size() && i < matrix.size() ; i++)
			yVals << (scaleAction->isChecked() ? item->sample(selectedPoints[i]).y() : 0 ) ;

		// do coefficient calculation
		coeffs = gaussjinv(matrix,yVals) ;

		int i = 0 ; // TODO make offset and offline supersede scale
		scale = scaleAction->isChecked() && i < coeffs.size() ? coeffs[i++] : 1. ,
				offset= offsetAction->isChecked()&& i < coeffs.size() ? coeffs[i++] : 0. ,
				offline=offlineAction->isChecked()&& i < coeffs.size() ? coeffs[i++] : 0. ;
	}
	specPlotMoveCommand *command = new specPlotMoveCommand ;
	command->setItem(modelItem) ;
	command->setCorrections(shift,offset,offline,scale) ;
	command->setParentObject(view->model()) ;
	undoPartner()->push(command) ;
}

void generatePointsInRange(const QwtPlotItemList& zeroRanges,
			   const specModelItem* spectrum,
			   QSet<double>& xValues)
{
	if (!spectrum) return ;
	// extract points that lie in any of the ranges
	for (size_t j = 0 ; j < spectrum->dataSize() ; ++j)
	{
		const QPointF& point = spectrum->sample(j) ;
		for (int k = 0 ; k < zeroRanges.size() ; ++k)
		{
			if (((specRange*) zeroRanges[k])->contains(point.x()))
			{
				xValues << point.x() ;
				break ; // only include each point once
			}
		}
	}

}

void generateReferenceSpectrum(const QMap<double,double>& referenceSpectrum,
				   const QSet<double>& xValues,
				   QMap<double, double>& refSpectrum)
{
	if (referenceSpectrum.isEmpty())
		foreach(const double& x, xValues)
			refSpectrum[x] = 0. ;
	else
	{
		foreach(const double& x, xValues)
		{
			if(referenceSpectrum.contains(x))
				refSpectrum[x] = referenceSpectrum[x] ;
			else
			{
				QMap<double,double>::const_iterator pointAfter = referenceSpectrum.upperBound(x);
				// no points for lin interpol found -> take point from correction list
				if (pointAfter == referenceSpectrum.begin() || // no point before to interpolate with
						pointAfter == referenceSpectrum.end()) // no point after to interpolate with
					continue ;
				QMap<double,double>::const_iterator pointBefore  = pointAfter - 1;
				// subtract linear interpolation
				refSpectrum[x] = pointBefore.value()
						+ (pointAfter.value() - pointBefore.value()) /
						(pointAfter.key()   - pointBefore.key()  ) *
						(x - pointBefore.key()) ;
			}
		}
	}
}

// Here come the macros to boost the performance
#define ACCUMULATE_ONES count += 1 ;
#define ACCUMULATE_X    x  += p.x() ;
#define ACCUMULATE_XX   xx += p.x()*p.x() ;
#define ACCUMULATE_XY   xy += p.x()*p.y() ;
#define ACCUMULATE_XZ   xz += p.x()*zv ;
#define ACCUMULATE_Y    y  += p.y() ;
#define ACCUMULATE_YZ   yz += p.y()*zv ;
#define ACCUMULATE_YY   yy += p.y()*p.y() ;
#define ACCUMULATE_Z    z  += zv ;
#define LOOPPOINTS(LOOPCORE) \
	for (size_t i = 0 ; i < spectrum->dataSize() ; ++i) { \
	QPointF p = spectrum->sample(i) ; \
	if (refSpectrum.contains(p.x())) { \
	double zv = (refSpectrum[p.x()] - p.y()) ; \
	LOOPCORE \
	}}
#define MATRIX_VECTOR_ASSIGNMENT_TWO(MATRIX_A, MATRIX_B, MATRIX_C, VECTOR_A, VECTOR_B) \
	QVector<double> vec(2) ; \
	QVector<QVector<double> > matrix(2, vec) ; \
	matrix[0][0] = MATRIX_A ; \
	matrix[1][0] = matrix[0][1] = MATRIX_B ; \
	matrix[1][1] = MATRIX_C ; \
	vec[0] = VECTOR_A ; \
	vec[1] = VECTOR_B ; \
	QVector<double> correction = gaussjinv(matrix, vec) ;

specMultiCommand * specSpectrumPlot::generateCorrectionCommand(const QwtPlotItemList &zeroRanges,
								   const QwtPlotItemList &spectra,
								   const QMap<double, double> &referenceSpectrum,
								   specModel *model,
								   bool calcOffset,
								   bool calcSlope,
								   bool calcScale)
{
	if (!calcOffset && !calcSlope) return 0 ; // Nur scale macht keinen Sinn (Referenz wird skaliert)
	specMultiCommand *zeroCommand = new specMultiCommand ;
	zeroCommand->setParentObject(model);

	QSet<double> xValues ;

	// Extract points in range and xValues needed (might be a bit heavy on the memory for storing all points)
	for (int i = 0 ; i < spectra.size() ; ++i)
		generatePointsInRange(zeroRanges, dynamic_cast<specModelItem*>(spectra[i]), xValues);

	// Generate correct reference spectrum
	QMap<double, double> refSpectrum ;
	generateReferenceSpectrum(referenceSpectrum, xValues, refSpectrum);

	typedef QVector3D refPoint ;

	foreach (QwtPlotItem* s, spectra)
	{
		specModelItem *spectrum = dynamic_cast<specModelItem*>(s) ;
		if (!spectrum) continue ;
		double count = 0, x = 0, xx = 0, y = 0, yy = 0, z = 0, xy = 0, xz = 0, yz = 0 ;
		double offset = 0, slope = 0 ;

		if (calcOffset && !calcSlope && !calcScale)
		{
			LOOPPOINTS(ACCUMULATE_ONES ACCUMULATE_Z) ;
			offset = z / count ;
		}
		else if (!calcOffset && calcSlope && !calcScale)
		{
			LOOPPOINTS(ACCUMULATE_XX ACCUMULATE_XZ) ;
			slope = xz/xx;
		}
		else if (calcOffset && calcSlope && !calcScale)
		{
			LOOPPOINTS(ACCUMULATE_ONES ACCUMULATE_X ACCUMULATE_XX ACCUMULATE_XZ ACCUMULATE_Z) ;
			MATRIX_VECTOR_ASSIGNMENT_TWO(count, x, xx, z, xz) ;
			offset = correction[0] ;
			slope  = correction[1] ;
		}
		else if (calcOffset && !calcSlope && calcScale)
		{
			LOOPPOINTS(ACCUMULATE_ONES ACCUMULATE_Y ACCUMULATE_YY ACCUMULATE_YZ ACCUMULATE_Z) ;
			MATRIX_VECTOR_ASSIGNMENT_TWO(count, y, yy, z, yz) ;
			if (correction[1] != 1.)
				offset = correction[0]/(1.- correction[1]) ; // TODO check or disable (this is dangerous!)
		}
		else if (!calcOffset && calcSlope && calcScale)
		{
			LOOPPOINTS(ACCUMULATE_X ACCUMULATE_XY ACCUMULATE_YY ACCUMULATE_XZ ACCUMULATE_YZ)
					MATRIX_VECTOR_ASSIGNMENT_TWO(x, xy, yy, xz, yz) ;
			if (correction[1] != 1.)
				slope = correction[0]/(1.-correction[1]) ; // TODO check or disable (this is dangerous!)
		}
		else if (calcOffset && calcSlope && calcScale)
		{
			LOOPPOINTS(ACCUMULATE_ONES ACCUMULATE_X  ACCUMULATE_Y  ACCUMULATE_Z
				   ACCUMULATE_XX   ACCUMULATE_XY ACCUMULATE_YY
				   ACCUMULATE_XZ   ACCUMULATE_YZ) ;
			QVector<double> vec(3) ;
			QVector<QVector<double> > matrix(3, vec) ;
			matrix[0][0] = count ;
			matrix[1][1] = xx ;
			matrix[2][2] = yy ;
			matrix[0][1] = matrix[1][0] = x ;
			matrix[0][2] = matrix[2][0] = y ;
			matrix[1][2] = matrix[2][1] = xy;
			vec[0] = z ;
			vec[1] = xz ;
			vec[2] = yz ;
			QVector<double> correction = gaussjinv(matrix, vec) ;
			if (correction[2] != -1.)
			{
				offset = correction[0]/(1.+correction[2]) ;
				slope  = correction[1]/(1.+correction[2]) ;
			}
		}

		specPlotMoveCommand *command = new specPlotMoveCommand(zeroCommand) ;
		command->setItem(spectrum);
		command->setCorrections(0, offset, slope, 1.) ;
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

	specMultiCommand *zeroCommand = generateCorrectionCommand(zeroRanges,
								  spectra,
								  referenceSpectrum,
								  view->model(),
								  offsetAction->isChecked(),
								  offlineAction->isChecked(),
								  scaleAction->isChecked()) ;
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
			command->setItem(spectrum,data);
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
				   << alignWithReferenceAction
				   << alignmentActions->actions()
				   << subInterpolatedAction ;
}

specSpectrumPlot::~specSpectrumPlot()
{
	if (correctionPicker) correctionPicker->purgeSelectable();
	delete correctionPicker ;
	delete alignmentPicker ;
}
