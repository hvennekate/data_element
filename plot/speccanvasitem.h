#ifndef SPECCANVASITEM_H
#define SPECCANVASITEM_H

#include <qwt_plot_curve.h>
#include <QList>
#include <QPair>
#include <qwt_interval.h>
#include "names.h"
#include <QMenu>
#include <qwt_symbol.h>
#include "specstreamable.h"
#include "specdatapointfilter.h"

class specCanvasItem : public QwtPlotCurve, public specStreamable
{
private:
	QList<int> selectedPoints ;
	QwtSymbol* oldSymbol ;
	class moveIndicator : public QwtSymbol
	{
	private:
		QSize boundingSize() const ;
		void renderSymbols(QPainter*, const QPointF*, int numPoints) const ;
		QRect boundingRect() const ;
	};
	virtual void initializeData() ;
protected:
	void writeToStream(QDataStream& out) const;
	void readFromStream(QDataStream& in) ;
public:
	specCanvasItem(QString description = "");
	virtual void pointMoved(const int&, const double&, const double&) {}
	virtual void refreshPlotData() = 0;
	virtual void addDataFilter(const specDataPointFilter&) {}
	virtual void attach(QwtPlot* plot) { QwtPlotCurve::attach(plot) ; }
	virtual void detach() { QwtPlotCurve::detach() ; }
	virtual QMenu* contextMenu() ;
	~specCanvasItem();
	virtual QMap<double,double> dataMap() ;
	virtual QVector<double> xVector() ;
	virtual QVector<double> yVector() ;
	virtual QVector<QPointF> dataVector() ;

	int rtti() const { return spec::canvasItem ; }
	virtual void highlight(bool highlight) ;

	virtual void setLineWidth(const double&) ;
	virtual double lineWidth() const;
	virtual QColor penColor() const;
	virtual void setPenColor(const QColor&) ;
	virtual int symbolStyle() const ;
	virtual void setSymbolStyle(const int&) ;
	virtual QColor symbolPenColor() const ;
	virtual void setSymbolPenColor(const QColor&) ;
	virtual void setSymbolBrushColor(const QColor&) ;
	virtual QColor symbolBrushColor() const ;
	virtual QSize symbolSize() const;
	virtual void setSymbolSize(int w, int h = -1) ;
	virtual void setSymbolSize(const QSize&) ;
	virtual qint8 penStyle() const;
	virtual void setPenStyle(const qint8&) ;
};

#endif
