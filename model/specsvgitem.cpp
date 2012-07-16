#include "specsvgitem.h"
#include <qwt/qwt_plot.h>

specSVGItem::specSVGItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  image(0),
	  data(0),
	  highlighting(0),
	  width(-1),
	  height(-1),
	  fix(center)
{
	width = 100 ;
	height = 100 ;
	setStyle(QwtPlotCurve::NoCurve);
}

void specSVGItem::setImage(const QRectF &rect, const QByteArray &newData)
{
	if (data) delete data ;
	data = new QByteArray(newData) ;
	if (!image)
		image = new QwtPlotSvgItem ;
	image->loadData(rect,*data) ;
}



void specSVGItem::attach(QwtPlot *plot)
{
	if (image)
		image->attach(plot) ;
	specCanvasItem::attach(plot) ;
	highlight(highlighting) ;
}

void specSVGItem::setBoundingRect(const QRectF &rect)
{
	qDebug() << "Setting SVG bounding rect" << rect << rect.width() << rect.height();
	if (!data || !image) return ;
	image->loadData(rect,*data) ;
	highlight(highlighting) ;
}

void specSVGItem::highlight(bool highlight)
{
	highlighting = highlight ;
	if (!image) return ;
	QVector<QPointF> points(specSVGItem::size) ;
	for (int i = 0 ; i < specSVGItem::size ; ++i)
		points[i] = anchorPoint((SVGCornerPoints) i) ;
	qDebug() << "bounding points:" << points ;
	setSamples(points) ;
	if (!highlight)
		setSymbol(0) ;
	else
		setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::black),QPen(Qt::white),QSize(5,5))) ;
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
	}
}

void specSVGItem::refreshSVG(double xfactor, double yfactor)
{
	qDebug() << "Refreshing SVG plot data" ;
	if (!plot()) return ;
	if (!image) return ;
	QPointF fixPoint = anchorPoint(fix) ;
	QRectF bounds = image->boundingRect() ;
	qDebug() << "bottom" << plot()->canvasMap(QwtPlot::xBottom).sDist() << plot()->canvasMap(QwtPlot::xBottom).pDist() ;
	if (width  >= 0) bounds.setWidth(width * xfactor) ;
	if (height >= 0) bounds.setHeight(height * yfactor) ;
	if (fix >= 0)    setAnchor(bounds, fixPoint, fix) ;
	setBoundingRect(bounds) ;
}

void specSVGItem::writeToStream(QDataStream &out) const
{
	specModelItem::writeToStream(out) ;
	out << quint8(highlighting) << qint16(width) << qint16(height) << qint16(fix)
	    << image->boundingRect() << *data ;
}

void specSVGItem::readFromStream(QDataStream &in) const
{
	specModelItem::readFromStream(in) ;
	quint8 hl ;
	qint16 w, h, f ;
	delete data ;
	delete image ;
	in >> hl >> w >> h >> f ;
	highlighting = hl ;
	width = w ;
	height = h ;
	fix = f ;
	QRectF rect ;
	image = new QwtPlotSvgItem ;
	data = new QByteArray ;
	in >> rect >> *data ;
	// TODO refresh
}
