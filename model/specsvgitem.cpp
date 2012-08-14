#include "specsvgitem.h"
#include <qwt/qwt_plot.h>

void specSVGItem::attach(QwtPlot *plot)
{
	if (image)
		image->attach(plot) ;
	specCanvasItem::attach(plot) ;
	highlight(highlighting) ;
}

void specSVGItem::highlight(bool highlight)
{
	highlighting = highlight ;
	if (!image) return ;
	QVector<QPointF> points(specSVGItem::size) ;
	for (int i = 0 ; i < specSVGItem::size ; ++i)
		points[i] = anchorPoint((SVGCornerPoints) i) ;
	setSamples(points) ;
	if (!highlight)
		setSymbol(0) ;
	else
		setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::black),QPen(Qt::white),QSize(5,5))) ;
}

double specSVGItem::transform(const dimension& value)
{
	if (plot())
		return plot()->invTransform(value.first, value.second) ;
	return dimension.second ;
}

specSVGItem::specSVGItem(specFolderItem *par, QString description, double w, double h, double xp, double yp)
	: specModelItem(par,description),
	  highlighting(0),
	  anchor(center),
	  width(qMakePair(QwtPlot::axisCnt,0)),
	  height(qMakePair(QwtPlot::axisCnt,0)),
	  x(qMakePair(QwtPlot::axisCnt,0)),
	  y(qMakePair(QwtPlot::axisCnt,0)),
	  aspect(qMakePair(true,1.))
{
	setStyle(QwtPlotCurve::NoCurve);
}

void specSVGItem::setImage(const QByteArray &newData)
{
	data = newData ;
	redrawImage();
}

void specSVGItem::redrawImage()
{
	double nw = xPixels ?  : width,
	       nh = yPixels ?  : height ;
}





void specSVGItem::setBoundingRect(const QRectF &rect)
{
	if (!data || !image) return ;
	image->loadData(rect,*data) ;
	highlight(highlighting) ;
}

QPointF specSVGItem::anchorPoint(const SVGCornerPoints &point) const
{
//	if (point == undefined) return QPointF() ;
	QRectF bounds = image->boundingRect() ;
	switch(point)
	{
	case center: return bounds.center() ;
	case left: return (bounds.topLeft()+bounds.bottomLeft())/2. ;
	case right: return (bounds.topRight()+bounds.bottomRight())/2. ;
	case top: return (bounds.topLeft()+bounds.topRight())/2. ;
	case bottom: return (bounds.bottomLeft()+bounds.bottomRight())/2. ;
	case topLeft: return bounds.topLeft() ;
	case bottomLeft: return bounds.bottomLeft() ;
	case topRight: return bounds.topRight() ;
	case bottomRight: return bounds.bottomRight() ;
	default: return QPointF() ;
	}
}

void specSVGItem::setAnchor(QRectF& bounds, const QPointF & anchor, const SVGCornerPoints &point) const
{
	switch(point)
	{
	case center:
	case right:
	case left:
	case bottom:
	case top:
		 bounds.moveCenter(anchor) ;
	default: ;
	}

	switch (point)
	{
	case center: bounds.moveCenter(anchor) ; break ;
	case right: bounds.moveRight(anchor.x()) ; break ;
	case left: bounds.moveLeft(anchor.x()) ; break ;
	case bottom: bounds.moveBottom(anchor.y()) ; break ;
	case top: bounds.moveTop(anchor.y()) ; break ;
	case topLeft: bounds.moveTopLeft(anchor) ; break ;
	case topRight: bounds.moveTopRight(anchor) ; break ;
	case bottomLeft: bounds.moveBottomLeft(anchor) ; break ;
	case bottomRight: bounds.moveBottomRight(anchor) ; break ;
	default: ;
	}
}

void specSVGItem::refreshSVG(double xfactor, double yfactor)
{
	if (!plot()) return ;
	if (!image) return ;
	QPointF fixPoint = anchorPoint(fix) ;
	QRectF bounds = image->boundingRect() ;
	if (width  >= 0) bounds.setWidth(width * xfactor) ;
	if (height >= 0) bounds.setHeight(height * yfactor) ;
	if (fix >= 0)    setAnchor(bounds, fixPoint, fix) ;
	setBoundingRect(bounds) ;
}

void specSVGItem::writeToStream(QDataStream &out) const
{
	specModelItem::writeToStream(out) ;
	out << quint8(highlighting) << qint16(width) << qint16(height) << qint8(fix)
	    << image->boundingRect() << *data ;
}

void specSVGItem::readFromStream(QDataStream &in)
{
	specModelItem::readFromStream(in) ;
	quint8 hl ;
	qint8 f ;
	qint16 w, h ;
	delete data ;
	delete image ;
	in >> hl >> w >> h >> f ;
	highlighting = hl ;
	width = w ;
	height = h ;
	fix = SVGCornerPoints(f) ;
	QRectF rect ;
	image = new QwtPlotSvgItem ;
	data = new QByteArray ;
	in >> rect >> *data ;
	// TODO refresh
}
