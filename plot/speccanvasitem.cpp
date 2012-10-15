#include "speccanvasitem.h"
#include "utility-functions.h"
#include "specplot.h"
#include <QTextStream>
#include <QInputDialog>
#include <qwt_symbol.h>
#include "specplotstyle.h"
#include <QPainter>

specCanvasItem::specCanvasItem( QString description)
		: QwtPlotCurve(description),
		  oldSymbol(0)
{
}

specCanvasItem::~specCanvasItem()
{
	detach() ;
	delete oldSymbol ;
}

void specCanvasItem::setLineWidth(const double& w)
{
	QPen newPen(pen()) ;
	newPen.setWidthF(w) ;
	setPen(newPen) ;
}

double specCanvasItem::lineWidth() const
{
	return pen().widthF() ;
}

QMenu* specCanvasItem::contextMenu()
{ return 0 ;} // TODO

void specCanvasItem::highlight(bool highlight)
{
	if (highlight)
	{
		if (!oldSymbol && symbol())
			oldSymbol = new QwtSymbol(*(symbol())) ;
		moveIndicator *indicator = new moveIndicator ;
		indicator->setStyle(QwtSymbol::Cross);
		setSymbol(indicator) ;
	}
	else
	{
		setSymbol(oldSymbol) ;
		oldSymbol = 0 ; // QwtPlotCurve assumes ownership
	}
}

QColor specCanvasItem::penColor() const
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

int specCanvasItem::symbolStyle() const
{
	if (oldSymbol)
		return oldSymbol->style() ;
	if (symbol())
		return symbol()->style() ;
	return specPlotStyle::noSymbol ;
}

void specCanvasItem::setSymbolStyle(const int& newStyle)
{
	if (oldSymbol)
	{
		if (specPlotStyle::noSymbol == newStyle)
		{
			delete oldSymbol ;
			oldSymbol = 0 ;
		}
		else
			oldSymbol->setStyle(QwtSymbol::Style(newStyle)) ;
	}
	else
	{
		if (specPlotStyle::noSymbol == newStyle)
			setSymbol(0) ;
		else
		{
			QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
			newSymbol->setStyle(QwtSymbol::Style(newStyle)) ;
			setSymbol(newSymbol) ;
		}
	}
}

QColor specCanvasItem::symbolPenColor() const
{
	if (oldSymbol) return oldSymbol->pen().color() ;
	if (!symbol()) return QColor() ;
	return symbol()->pen().color() ;
}

void specCanvasItem::setSymbolPenColor(const QColor& newColor)
{
	if (oldSymbol)
	{
		QPen newPen(oldSymbol->pen()) ;
		newPen.setColor(newColor) ;
		oldSymbol->setPen(newPen) ;
	}
	else
	{
		QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
		QPen newPen = newSymbol->pen() ;
		newPen.setColor(newColor) ;
		newSymbol->setPen(newPen) ;
		setSymbol(newSymbol) ;
	}
}

void specCanvasItem::setSymbolBrushColor(const QColor &newColor)
{
	if (oldSymbol)
	{
		QBrush brush(oldSymbol->brush()) ;
		brush.setColor(newColor) ;
		oldSymbol->setBrush(brush) ;
	}
	else
	{
		QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
		newSymbol->setBrush(newColor) ;
		setSymbol(newSymbol) ;
	}
}

QColor specCanvasItem::symbolBrushColor() const
{
	if (oldSymbol) return oldSymbol->brush().color() ;
	if (!symbol()) return QColor() ;
	return symbol()->brush().color() ;
}

QSize specCanvasItem::symbolSize() const
{
	if (oldSymbol) return oldSymbol->size() ;
	if (!symbol()) return QSize() ;
	return symbol()->size() ;
}

void specCanvasItem::setSymbolSize(int w, int h)
{
	if (h <0) setSymbolSize(QSize(w,w)) ;
	else setSymbolSize(QSize(w,h)) ;
}

void specCanvasItem::setSymbolSize(const QSize& s)
{
	if (oldSymbol)
	{
		oldSymbol->setSize(s) ;
	}
	else
	{
		QwtSymbol *newSymbol = symbol() ? (new QwtSymbol(*symbol())) : (new QwtSymbol()) ;
		newSymbol->setSize(s) ;
		setSymbol(newSymbol) ;
	}
}

void specCanvasItem::moveIndicator::drawSymbols(QPainter *painter, const QPointF *points, int numPoints) const
{
	QPixmap pixmap(":/moveIndicator.png", "PNG") ;
	QRectF frame(-5,-5,11,11) ;
	for (int i = 0 ; i < numPoints ; ++i)
	{
		frame.moveCenter(points[i]) ;
		painter->drawPixmap(frame,pixmap,pixmap.rect());
	}
}

QSize specCanvasItem::moveIndicator::boundingSize() const
{
	return QSize(11,11) ;
}

qint8 specCanvasItem::penStyle() const
{
//	if (style() != QwtPlotCurve::Lines) return -1 ;
	return pen().style() ;
}

void specCanvasItem::setPenStyle(const qint8 &s)
{
//	if (s < 0)
//	{
//		setStyle(QwtPlotCurve::NoCurve) ;
//		return ;
//	}
//	else
//		setStyle(QwtPlotCurve::Lines) ;
	QPen newPen(pen()) ;
	newPen.setStyle((Qt::PenStyle) s) ;
	setPen(newPen) ;
}
