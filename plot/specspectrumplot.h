#ifndef SPECSPECTRUMPLOT_H
#define SPECSPECTRUMPLOT_H

#include "specplot.h"
#include <QHash>
#include <QList>

class QActionGroup ;
class specMultiCommand ;
class specRange ;
class specDataItem ;
class specModelItem ;
class specModel ;

class specSpectrumPlot : public specPlot
{
	Q_OBJECT
private:
	QAction *offsetAction,
		*offlineAction,
		*scaleAction,
		*shiftAction,
		*setReferenceAction,
		*alignWithReferenceAction,
		*addRangeAction,
		*removeRangeAction,
		*noSlopeAction,
		*subInterpolatedAction ;
	QActionGroup *correctionActions, *alignmentActions ;
	CanvasPicker *correctionPicker, *alignmentPicker ;
	QHash<specCanvasItem*, QList<int> > pointHash ;
	QList<specRange*> zeroRanges ;
	specDataItem *reference ;
	void toggleAligning(bool on=true) ;
	void invalidateReference() ;


	bool correctionChecked() ;

	QList<specDataItem*> folderContent(specModelItem*) ;

public:
	explicit specSpectrumPlot(QWidget *parent = 0);
	~specSpectrumPlot() ;
	QList<QAction*> actions() ;
	static specMultiCommand* generateCorrectionCommand(const QwtPlotItemList& zeroRanges, const QwtPlotItemList& spectra, const QMap<double, double>& referenceSpectrum, specModel*, bool noSlope = false) ;

	void attachToPicker(specCanvasItem*) ;
	void detachFromPicker(specCanvasItem*) ;
signals:

private slots:
	void correctionsChanged() ;
	void alignmentChanged(QAction*) ;
	void pointMoved(specCanvasItem*,int point, double x, double y) ;
	void applyZeroRanges(specCanvasItem* range,int point, double x, double y) ;
	void multipleSubtraction() ;
	void setReference() ;
};

#endif // SPECSPECTRUMPLOT_H
