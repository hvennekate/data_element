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
{
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
	specPlot *plot() { return (specPlot *)parent(); }

	specCanvasItem *d_selectedCurve, *lastSelected;
	int d_selectedPoint;

public:
	explicit CanvasPicker(specPlot *plot);
	virtual bool eventFilter(QObject *, QEvent *);

	virtual bool event(QEvent *);
	specCanvasItem* current() ;

signals:
	void moved(specCanvasItem*) ;
	void pointMoved(specCanvasItem*, int, double, double) ;
};

#endif
