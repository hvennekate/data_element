#ifndef SPECSPECTRUMPLOT_H
#define SPECSPECTRUMPLOT_H

#include "specplot.h"
#include <QHash>
#include <QList>
#include <QActionGroup>
#include "actionlib/specactionlibrary.h"
#include "specview.h"
#include "actionlib/specundoaction.h"

class specMultiCommand ;

class specSpectrumPlot : public specPlot
{
	Q_OBJECT
private:
	QAction *offsetAction,
		*offlineAction,
		*scaleAction,
		*shiftAction,
		*printAction,
		*setReferenceAction,
		*alignWithReferenceAction,
		*addRangeAction,
		*removeRangeAction,
		*noSlopeAction,
		*modifySVGs,
		*subInterpolatedAction ;
	QActionGroup *correctionActions, *alignmentActions ;
	CanvasPicker *correctionPicker, *alignmentPicker, *SVGpicker ;
	specActionLibrary *undoPartner ;
	QHash<specCanvasItem*, QList<int> > pointHash ;
	QList<specRange*> zeroRanges ;
	specView *view ;
	specDataItem *reference ;
	void toggleAligning(bool on=true) ;
	void invalidateReference() ;


	bool correctionChecked() ;

	QList<specDataItem*> folderContent(specModelItem*) ;

public:
	explicit specSpectrumPlot(QWidget *parent = 0);
	QList<QAction*> actions() { return QList<QAction*>() << correctionActions->actions() << setReferenceAction << alignmentActions->actions() << printAction << modifySVGs; }
	void setView(specView* mod) { view = mod ; }
	void setUndoPartner(specActionLibrary* lib) { undoPartner = lib ; ((specUndoAction*) printAction)->setLibrary(lib);}
	static specMultiCommand* generateCorrectionCommand(const QwtPlotItemList& zeroRanges, const QwtPlotItemList& spectra, const QMap<double, double>& referenceSpectrum, specView*, bool noSlope = false) ;
signals:

private slots:
	void correctionsChanged() ;
	void alignmentChanged(QAction*) ;
	void pointMoved(specCanvasItem*,int point, double x, double y) ;
	void applyZeroRanges(specCanvasItem* range,int point, double x, double y) ;
	void resizeSVG(specCanvasItem*, int point, double x, double y) ;
	void modifyingSVGs(const bool&) ;
	void multipleSubtraction() ;
};

#endif // SPECSPECTRUMPLOT_H
