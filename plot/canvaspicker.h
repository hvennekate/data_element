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
// 	QList<specCanvasItem*> selectable ;
	void select(const QPoint &);
	void move(const QPoint &);
	void moveBy(int dx, int dy);
	void release();
	void showCursor(bool enable);
	void shiftPointCursor(bool up);
	void shiftCurveCursor(bool up);
	void movePointExplicitly() ;
	specPlot *plot() { return (specPlot *)parent(); }
//     const QwtPlot *plot() const { return (QwtPlot *)parent(); }

	specCanvasItem *d_selectedCurve, *lastSelected;
	int d_selectedPoint;
// 	spec::moveMode mode ;
// 	QList<specRange*> ranges;
// 	QList<specRange*>::size_type selectedRange ;
// 	void setRange() ;
public:
	CanvasPicker(specPlot *plot);
	virtual bool eventFilter(QObject *, QEvent *);

	virtual bool event(QEvent *);
	specCanvasItem* current() ;
// 	void setSelectable(QList<specCanvasItem*>) ;
// 	bool removeItem(specCanvasItem*) ;
// 	void removeAll() ;
// 	void setMoveMode(spec::moveMode) ;
// 	void enableMoveMode(spec::moveMode) ;
// 	void disableMoveMode(spec::moveMode) ;
// public slots:
// 	void addRange() ;
// 	void deleteRange() ;
// signals:
// 	void rangesModified(QList<specRange*>*) ;
// 	void haveZeroRanges(bool) ;
signals:
	void moved(specCanvasItem*) ;
};

#endif
