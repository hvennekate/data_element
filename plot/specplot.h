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

class specPlot : public QwtPlot
{
		Q_OBJECT
	private:
		QList<specCanvasItem*>* canBeSelected ;
		QList<specCanvasItem*>* ranges ;
		QList<specCanvasItem*>* ordinary ;
		QList<specCanvasItem*>* kineticRanges ;
		QList<specCanvasItem*>* selectRanges ;
		CanvasPicker *pick ;
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
		void setupActions() ;
		void highlightSelectable(bool highlight=true) ;
	public:
		bool select ;
		QList<specCanvasItem*>* selectable() ;
		specPlot ( QWidget *parent=NULL );
		~specPlot();
		CanvasPicker *picker() ;
		specZoomer   *zoomer() ;
		QList<QAction*> actions() ;
		spec::moveMode moveMode() ;
		void refreshRanges() ;
	signals:
		void changed() ;
	public slots :
		void replot() ;
		void changeTitle() ;
		void changeXLabel();
		void changeYLabel();
		void refreshMoveMode() ;
		void addRange() ;
		void deleteRange() ;
};

#endif
