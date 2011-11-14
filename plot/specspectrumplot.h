#ifndef SPECSPECTRUMPLOT_H
#define SPECSPECTRUMPLOT_H

#include "specplot.h"
#include <QHash>
#include <QList>
#include <QActionGroup>
#include "actionlib/specactionlibrary.h"
#include "specview.h"
#include "actionlib/specundoaction.h"

class specSpectrumPlot : public specPlot
{
	Q_OBJECT
private:
	QAction *offsetAction, *offlineAction, *scaleAction, *shiftAction, *printAction ;
	QAction *setReferenceAction, *alignWithReferenceAction, *addRangeAction, *removeRangeAction, *noSlopeAction ;
	QActionGroup *correctionActions, *alignmentActions ;
	CanvasPicker *picker ;
	specActionLibrary *undoPartner ;
	QHash<specCanvasItem*, QList<int> > pointHash ;
	QList<specRange*> zeroRanges ;
	specView *view ;
	specDataItem *reference ;

	bool correctionChecked() ;

	QList<specDataItem*> folderContent(specModelItem*) ;

public:
	explicit specSpectrumPlot(QWidget *parent = 0);
	QList<QAction*> actions() { return QList<QAction*>() << correctionActions->actions() << alignmentActions->actions() << printAction ; }
	void setView(specView* mod) { view = mod ; }
	void setUndoPartner(specActionLibrary* lib) { qDebug("setting action lib") ; undoPartner = lib ; ((specUndoAction*) printAction)->setLibrary(lib);}
signals:

private slots:
	void correctionsChanged() ;
	void alignmentChanged(QAction*) ;
	void pointMoved(specCanvasItem*,int point, double x, double y) ;
	void applyZeroRanges(specCanvasItem*,int point, double x, double y) ;

};

#endif // SPECSPECTRUMPLOT_H
