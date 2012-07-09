#ifndef SPECPLOTSTYLE_H
#define SPECPLOTSTYLE_H
#include <QPen>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include "specinstream.h"
#include "specoutstream.h"

class specPlotStyle
{
	QPen pen, symbolPen ;
	qint16 symbolType ;
	QBrush symbolBrush ;
	QSize symbolSize ;
	void initialize(const QwtPlotCurve*) ;
public:
	explicit specPlotStyle(specInStream&);
	explicit specPlotStyle(const QwtPlotCurve*);
	void apply(QwtPlotCurve*) const;
	void retrieve(QwtPlotCurve*) ;
	void write(specOutStream&) const ;
	bool read(specOutStream&) const ;
};

#endif // SPECPLOTSTYLE_H
