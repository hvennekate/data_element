#include "specplotstyle.h"
#include "names.h"

specPlotStyle::specPlotStyle(QDataStream& in)
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

void specPlotStyle::writeToStream(QDataStream &out) const
{
	out << pen << symbolPen << symbolType << symbolBrush << symbolSize ;
}

void specPlotStyle::readFromStream(QDataStream &in)
{
	in >> pen >> symbolPen >> symbolType >> symbolBrush >> symbolSize ;
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
