#ifndef CANVASPICKER_H
#define CANVASPICKER_H

#include <QObject>
#include <QHash>
#include <QAbstractItemModel>
#include <specmodelitem.h>
#include <names.h>
#include <specrange.h>
#include <QSet>

class QPoint;
class QCustomEvent;
class specPlot;
// class QwtPlotCurve;
class specCanvasItem;

/*! Class for handling plot interactions other than zooming (taken from Qwt library's example code) */
class CanvasPicker: public QObject
{ // TODO subclass different pickers for zero ranges...
    Q_OBJECT
private:
	void select(const QPoint &);
	void move(const QPoint &);
	void moveBy(int dx, int dy);
	void release();
	void showCursor(bool enable);
	void shiftPointCursor(bool up);
	void shiftCurveCursor(bool up);
	void movePointExplicitly() ;
	bool owning ;
	specPlot *plot() { return (specPlot *)parent(); }
	QSet<specCanvasItem*> selectable ;
	void highlightSelectable() ;
	void switchHighlighting(bool) ;

	specCanvasItem *d_selectedCurve, *lastSelected;
	int d_selectedPoint;
	bool highlighting ;

public:
	explicit CanvasPicker(specPlot *plot);
	~CanvasPicker() ;
	virtual bool eventFilter(QObject *, QEvent *);	
	virtual bool event(QEvent *);
	void addSelectable(const QSet<specCanvasItem*>&) ;
	void addSelectable(specCanvasItem*) ;
	void removeSelectable(QSet<specCanvasItem*>&) ;
	void removeSelectable(specCanvasItem*) ;
	void removeSelectable();
	void setSelectable(const QSet<specCanvasItem*>&) ;
	void removeSelected() ;
	int countSelectable() const ;
	void highlightSelectable(bool) ;
	QList<specCanvasItem*> items() const ;
	inline void setOwning(bool Owning =true) { owning = Owning ; } // TODO do this more skilfully.
	void purgeSelectable() ;

signals:
	void pointMoved(specCanvasItem*, int, double, double) ;
};

#endif
