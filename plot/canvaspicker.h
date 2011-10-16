#ifndef CANVASPICKER_H
#define CANVASPICKER_H

#include <QObject>
#include <QHash>
#include <QAbstractItemModel>
#include <specmodelitem.h>
#include <names.h>
#include <specrange.h>

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
	QList<specCanvasItem*> selectable ;
	void highlightSelectable(bool) ;

	specCanvasItem *d_selectedCurve, *lastSelected;
	int d_selectedPoint;

public:
	explicit CanvasPicker(specPlot *plot);
	~CanvasPicker() ;
	virtual bool eventFilter(QObject *, QEvent *);

	virtual bool event(QEvent *);
	specCanvasItem* current() ;
	inline const QList<specCanvasItem*> getSelectable() { return selectable ;}
	void addSelectable(QList<specCanvasItem*>&) ;
	void addSelectable(specCanvasItem*) ;
	void removeSelectable(QList<specCanvasItem*>&) ;
	void removeSelectable(specCanvasItem*) ;
	void removeSelected() ;
	inline void setOwning(bool Owning =true) { owning = Owning ; } // TODO do this more skilfully.

signals:
	void moved(specCanvasItem*) ;
	void pointMoved(specCanvasItem*, int, double, double) ;
};

#endif
