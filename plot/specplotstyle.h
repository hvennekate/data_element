#ifndef SPECPLOTSTYLE_H
#define SPECPLOTSTYLE_H
#include <QPen>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include "specstreamable.h"

class specPlotStyle : public specStreamable
{
	QPen pen, symbolPen ;
	qint16 symbolType ;
	QBrush symbolBrush ;
	QSize symbolSize ;
	void initialize(const QwtPlotCurve*) ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	specStreamable::type id() const { return specStreamable::plotStyle ; }
public:
	explicit specPlotStyle(QDataStream&);
	explicit specPlotStyle(const QwtPlotCurve*);
	void apply(QwtPlotCurve*) const;
	void retrieve(QwtPlotCurve*) ;
};

#endif // SPECPLOTSTYLE_H
