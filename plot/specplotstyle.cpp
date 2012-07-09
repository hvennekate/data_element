#include "specplotstyle.h"
#include "names.h"

specPlotStyle::specPlotStyle(specInStream& in)
	: symbolType(-1)
{
	read(in) ;
}

specPlotStyle::specPlotStyle(const QwtPlotCurve *c)
	: symbolType(-1)
{
	initialize(c) ;
}

void specPlotStyle::initialize(const QwtPlotCurve *curve)
{
	pen = curve->pen();
	if (curve->symbol())
	{
		symbolType = curve->symbol()->style() ;
		symbolPen  = curve->symbol()->pen() ;
		symbolBrush= curve->symbol()->brush() ;
		symbolSize = curve->symbol()->size() ;
	}
}

void specPlotStyle::write(specOutStream &out) const
{
	out.next(spec::plotStyle) ;
	return out << pen << symbolPen << symbolType << symbolBrush << symbolSize ;
}

bool specPlotStyle::read(specInStream &in)
{
	if (!in.expect(spec::plotStyle)) return ;
	in >> pen >> symbolPen >> symbolType >> symbolBrush >> symbolSize ;
	return true ;
}

void specPlotStyle::apply(QwtPlotCurve *c) const
{
	c->setPen(pen) ;
	if (symbolType == -1)
		c->setSymbol(0) ;
	else
		c->setSymbol(new QwtSymbol((QwtSymbol::Style) symbolType, symbolBrush, symbolPen, symbolSize));
}

void specPlotStyle::retrieve(QwtPlotCurve *c)
{
	initialize(c) ;
}
