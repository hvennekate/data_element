#ifndef SPECSPECTRUMPLOT_H
#define SPECSPECTRUMPLOT_H

#include "specplot.h"
#include <QHash>
#include <QList>
#include <QActionGroup>
#include "actionlib/specactionlibrary.h"
#include "specview.h"

class specSpectrumPlot : public specPlot
{
	Q_OBJECT
private:
	QAction *offsetAction, *offlineAction, *scaleAction, *shiftAction ;
	QAction *setReferenceAction, *alignWithReferenceAction, *addRangeAction, *removeRangeAction, *noSlopeAction ;
	QActionGroup *correctionActions, *alignmentActions ;
	CanvasPicker *picker ;
	specActionLibrary *undoPartner ;
	QHash<specCanvasItem*, QList<int> > pointHash ;
	specView *view ;

	bool correctionChecked() ;

public:
	explicit specSpectrumPlot(QWidget *parent = 0);
	QList<QAction*> actions() { return correctionActions->actions() ; }
	void setView(specView* mod) { view = mod ; }
	void setUndoPartner(specActionLibrary* lib) { undoPartner = lib ; }
signals:

private slots:
	void correctionsChanged() ;
	void alignmentChanged(QAction*) ;
	void pointMoved(specCanvasItem*,int point, double x, double y) ;

};

#endif // SPECSPECTRUMPLOT_H
