#ifndef SPECPLOT_H
#define SPECPLOT_H

#include <qwt_plot.h>
#include "canvaspicker.h"
#include "speczoomer.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QActionGroup>
#include "names.h"
#include "speccanvasitem.h"

class specMetaRange ;

class specPlot : public QwtPlot
{
	Q_OBJECT
private:
	bool replotting ;
	QList<specCanvasItem*>* canBeSelected ;
	QList<specCanvasItem*>* ranges ;
	QList<specCanvasItem*>* ordinary ;
	QList<specCanvasItem*>* kineticRanges ;
	QList<specCanvasItem*>* selectRanges ;
	QList<specCanvasItem*> metaRanges ;
	specZoomer   *zoom ;
	bool scaleX, scaleY ;
	void contextMenuEvent ( QContextMenuEvent* ) ;
	QMenu *contextMenu ;
	QAction *titleAction, *ylabelAction, *xlabelAction, *fixXAxisAction, *fixYAxisAction ;
	QAction *scaleAction,
	*offsetAction,
	*offlineAction,
	*shiftXAction,
	*zeroCorrectionAction,
	*addZeroRangeAction,
	*deleteZeroRangeAction,
	*individualZeroAction,
	*multipleCorrections ;
	QActionGroup *zero, *modifications ;
	spec::moveMode mm ;
	CanvasPicker *metaPicker ;
	void setupActions() ;
	void highlightSelectable(bool highlight=true) ;
private slots:
	void changeTitle(QString) ;
	void changeXLabel(QString) ;
	void changeYLabel(QString) ;
	void metaRangeMoved(specCanvasItem*, int, double, double) ;
public:
	bool select ;
	QList<specCanvasItem*>* selectable() ;
	specPlot ( QWidget *parent=NULL );
	~specPlot();
	specZoomer   *zoomer() ;
	virtual QList<QAction*> actions() ;
	spec::moveMode moveMode() ;
	void refreshRanges() ;
	virtual QDataStream& read(QDataStream& in) ;
	virtual QDataStream& write(QDataStream& out) const ;
signals:
	void changed() ;
public slots :
	void replot() ;
	void changeTextLabel() ;
	void refreshMoveMode() ;
	void addRange() ;
	void deleteRange() ;
};

#endif
