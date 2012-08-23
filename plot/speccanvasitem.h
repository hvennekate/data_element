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

class specCanvasItem : public QwtPlotCurve, public specStreamable
{
private:
	QList<int> selectedPoints ;
protected:
	void writeToStream(QDataStream &out) const;
	void readFromStream(QDataStream &in) ;
public:
	specCanvasItem(QString description="");
	virtual void pointMoved(const int&, const double&, const double&) ;
	virtual void applyRanges(QList<QwtInterval*>&) ;
	virtual void refreshPlotData() =0;
	virtual void scaleBy(const double&) {}
	virtual void addToSlope(const double&) {}
	virtual void moveYBy(const double&) {}
	virtual void moveXBy(const double&) {}
	virtual void attach(QwtPlot *plot) { QwtPlotCurve::attach(plot) ; }
	virtual void detach() { QwtPlotCurve::detach() ; }
	virtual QMenu* contextMenu() ;
	void setLineWidth(const double&) ;
	double lineWidth() ;
	~specCanvasItem();

	int rtti() const { return spec::canvasItem ; }
	virtual void highlight(bool highlight) ;

	QColor penColor() ;
	void setPenColor(const QColor&) ;
	int symbolStyle() ;
	void setSymbolStyle(const int&) ;
	QColor symbolPenColor() ;
	void setSymbolPenColor(const QColor&) ;
	void setSymbolBrushColor(const QColor&) ;
	QColor symbolBrushColor() ;
	QSize symbolSize() ;
	void setSymbolSize(int w, int h = -1) ;
	void setSymbolSize(const QSize&) ;
};

#endif
