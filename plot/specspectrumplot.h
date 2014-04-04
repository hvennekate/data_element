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
	QAction* offsetAction,
		 *offlineAction,
		 *scaleAction,
		 *shiftAction,
		 *setReferenceAction,
		 *alignWithReferenceAction,
		 *addRangeAction,
		 *removeRangeAction,
		 *subInterpolatedAction,
		 *applyRangesAction ;
	QActionGroup* correctionActions, *alignmentActions ;
	CanvasPicker* correctionPicker, *alignmentPicker ;

public:
	/*! Enum for specifying the mode of correction for spectra*/
	enum move { NoMoveMode = 0,
		    Offset = 1,
		    Scale  = 2,
		    Slope  = 4
		  } ;
	Q_DECLARE_FLAGS(moveMode, move)
	moveMode manualAlignment, rangeAlignment ;
private:
	QHash<specCanvasItem*, QList<int> > pointHash ;
	specDataItem* reference ;

	void invalidateReference() ;
	bool correctionChecked() ;
	QList<specDataItem*> folderContent(specModelItem*) ;
	moveMode correctionsStatus() const ;
	void setCorrectionsStatus(moveMode) ;
public:
	explicit specSpectrumPlot(QWidget* parent = 0);
	~specSpectrumPlot() ;
	QList<QAction*> actions() ;

	void attachToPicker(specCanvasItem*) ;
	void detachFromPicker(specCanvasItem*) ;
signals:

private slots:
	void correctionsChanged(QAction* action = 0) ;
	void alignmentChanged(QAction*) ;
	void pointMoved(specCanvasItem*, int point, double x, double y) ;
	void applyZeroRanges(specCanvasItem* range, int point, double x, double y) ;
	void applyZeroRanges() ;
	void multipleSubtraction() ;
	void setReference() ;
	void toggleAligning(bool on = true) ;
	void checkReferenceForScaling() ;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(specSpectrumPlot::moveMode)
#endif // SPECSPECTRUMPLOT_H
