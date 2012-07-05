#ifndef SPECPLOTSTYLE_H
#define SPECPLOTSTYLE_H
#include <QPen>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>


class specPlotStyle
{
	QPen pen, symbolPen ;
	qint16 symbolType ;
	QBrush symbolBrush ;
	QSize symbolSize ;
	void initialize(const QwtPlotCurve*) ;
public:
	explicit specPlotStyle(QDataStream&);
	explicit specPlotStyle(const QwtPlotCurve*);
	void apply(QwtPlotCurve*) const;
	void retrieve(QwtPlotCurve*) ;
	QDataStream& write(QDataStream& out) const ;
	QDataStream& read(QDataStream& in) ;
};

QDataStream& operator<<(QDataStream&, const specPlotStyle&) ;
QDataStream& operator>>(QDataStream&, specPlotStyle&) ;
#endif // SPECPLOTSTYLE_H
