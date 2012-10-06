#include "specplotstyle.h"
#include "names.h"

#define PLOTSTYLEINITIALIZER lineWidth(0),\
	penColor(Qt::black),\
	symbolPenColor(Qt::black),\
	symbolBrushColor(Qt::transparent),\
	symbolStyle(QwtSymbol::NoSymbol)

specPlotStyle::specPlotStyle()
	: PLOTSTYLEINITIALIZER
{
}

specPlotStyle::specPlotStyle(QDataStream& in)
	: PLOTSTYLEINITIALIZER

{
	readFromStream(in) ;
}

specPlotStyle::specPlotStyle(const specCanvasItem *c)
	: PLOTSTYLEINITIALIZER
{
	initialize(c) ;
}

void specPlotStyle::initialize(const specCanvasItem *curve)
{
	lineWidth = curve->lineWidth() ;
	penColor = curve->penColor() ;
	symbolStyle = curve->symbolStyle() ;
	if (symbolStyle != noSymbol)
	{
		symbolSize = curve->symbolSize() ;
		symbolPenColor = curve->symbolPenColor() ;
		symbolBrushColor = curve->symbolBrushColor() ;
	}
}

void specPlotStyle::writeToStream(QDataStream &out) const
{
	out << lineWidth
	    << penColor
	    << symbolStyle ;
	if (symbolStyle != noSymbol)
		out << symbolSize
		    << symbolPenColor
		    << symbolBrushColor ;
}

void specPlotStyle::readFromStream(QDataStream &in)
{
	in >> lineWidth
	   >> penColor
	   >> symbolStyle ;
	if (symbolStyle != noSymbol)
		in >> symbolSize
		    >> symbolPenColor
		    >> symbolBrushColor ;
}

void specPlotStyle::apply(specCanvasItem *c) const
{
	c->setLineWidth(lineWidth) ;
	c->setPenColor(penColor) ;
	c->setSymbolStyle(symbolStyle) ;
	c->setSymbolPenColor(symbolPenColor) ;
	c->setSymbolBrushColor(symbolBrushColor) ;
	c->setSymbolSize(symbolSize) ;
}

void specPlotStyle::retrieve(specCanvasItem *c)
{
	initialize(c) ;
}
