#include "speccanvasitem.h"
#include "utility-functions.h"
#include "specplot.h"
#include <QTextStream>
#include <QInputDialog>
#include <qwt_symbol.h>
#include "specplotstyle.h"

specCanvasItem::specCanvasItem( QString description)
		: QwtPlotCurve(description)
{
}

specCanvasItem::~specCanvasItem()
{
	specPlot *pointer = (specPlot*) plot() ;
	detach() ;
	if (pointer) pointer->replot() ;
}

void specCanvasItem::setLineWidth(const double& w)
{
	QPen newPen(pen()) ;
	newPen.setWidthF(w) ;
	setPen(newPen) ;
}

double specCanvasItem::lineWidth()
{
	return pen().widthF() ;
}

QMenu* specCanvasItem::contextMenu()
{ return 0 ;} // TODO

void specCanvasItem::highlight(bool highlight)
{
	setSymbol(highlight ? (new QwtSymbol(QwtSymbol::Ellipse,brush(),pen(),QSize(5,5))) : (new QwtSymbol()));
}

QColor specCanvasItem::penColor()
{
	return pen().color() ;
}

void specCanvasItem::writeToStream(QDataStream &out) const
{
	out << specPlotStyle(this) ;
}

void specCanvasItem::readFromStream(QDataStream &in)
{
	specPlotStyle style(this) ;
	in >> style ;
	style.apply(this) ;
}

void specCanvasItem::setPenColor(const QColor& newColor)
{
	QPen newPen(pen());
	newPen.setColor(newColor);
	setPen(newPen) ;
}

int specCanvasItem::symbolStyle()
{
	return symbol() ? symbol()->style() : -2 ;
}

void specCanvasItem::setSymbolStyle(const int& newStyle)
{
	if (newStyle == -2)
	{
		setSymbol(0) ;
		return ;
	}
	QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
	newSymbol->setStyle(QwtSymbol::Style(newStyle)) ;
	setSymbol(newSymbol) ;
}

QColor specCanvasItem::symbolPenColor()
{
	if (!symbol()) return QColor() ;
	return symbol()->pen().color() ;
}

void specCanvasItem::setSymbolPenColor(const QColor& newColor)
{
	qDebug() << "setting symbol pen color" << newColor ;
	QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
	QPen newPen = newSymbol->pen() ;
	newPen.setColor(newColor) ;
	newSymbol->setPen(newPen) ;
	setSymbol(newSymbol) ;
}

void specCanvasItem::setSymbolBrushColor(const QColor &newColor)
{
	QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
	newSymbol->setBrush(newColor) ;
	setSymbol(newSymbol) ;
}

QColor specCanvasItem::symbolBrushColor()
{
	if (!symbol()) return QColor() ;
	return symbol()->brush().color() ;
}

QSize specCanvasItem::symbolSize()
{
	if (!symbol()) return QSize() ;
	return symbol()->size() ;
}

void specCanvasItem::setSymbolSize(int w, int h)
{
	QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
	newSymbol->setSize(w,h) ;
	setSymbol(newSymbol) ;
}

void specCanvasItem::setSymbolSize(const QSize& s)
{
	QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
	newSymbol->setSize(s) ;
	setSymbol(newSymbol) ;
}
