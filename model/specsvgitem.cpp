#include "specsvgitem.h"
#include <qwt/qwt_plot.h>
#include <QSvgRenderer>

void specSVGItem::attach(QwtPlot *plot)
{
	image.attach(plot) ;
	specCanvasItem::attach(plot) ;
	highlight(highlighting) ;
}

void specSVGItem::highlight(bool highlight)
{
	highlighting = highlight ;
	QVector<QPointF> points(specSVGItem::size) ;
	QRectF br = boundRect() ;
	for (int i = 0 ; i < specSVGItem::size ; ++i)
		points[i] = getPoint((SVGCornerPoint) i,br) ;
	setSamples(points) ;
	if (!highlight)
		setSymbol(0) ;
	else
		setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::black),QPen(Qt::white),QSize(5,5))) ;
}

specSVGItem::specSVGItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  highlighting(false),
	  anchor(center)
{
	ownBounds.width=qMakePair<qint8,qreal>(QwtPlot::axisCnt,0) ;
	ownBounds.height=qMakePair<qint8,qreal>(QwtPlot::axisCnt,0) ;
	ownBounds.x=qMakePair<qint8,qreal>(QwtPlot::axisCnt,0) ;
	ownBounds.y=qMakePair<qint8,qreal>(QwtPlot::axisCnt,0) ;
	setStyle(QwtPlotCurve::NoCurve);
}

void specSVGItem::setImage(const QByteArray &newData)
{
	QSize defaultSize(QSvgRenderer(newData).defaultSize()) ;
	ownBounds.width = qMakePair<qint8,qreal>(QwtPlot::xBottom,defaultSize.width()) ;
	ownBounds.height = qMakePair<qint8,qreal>(QwtPlot::yLeft, defaultSize.height()) ;
	ownBounds.x = qMakePair<qint8,qreal>(QwtPlot::xBottom, 0) ;
	ownBounds.y = qMakePair<qint8,qreal>(QwtPlot::yLeft, 0) ;
	data = newData ;
}

QRectF specSVGItem::boundRect() const
{
	if (!plot()) return QRectF() ;
	qreal px = (ownBounds.x.first == QwtPlot::axisCnt) ? ownBounds.x.second :
			plot()->invTransform(ownBounds.x.first, ownBounds.x.second),
	       py = (ownBounds.y.first == QwtPlot::axisCnt) ? ownBounds.y.second :
			plot()->invTransform(ownBounds.y.first, ownBounds.y.second),
	       w = (ownBounds.width.first == QwtPlot::axisCnt) ? ownBounds.width.second :
			(plot()->invTransform(ownBounds.width.first, ownBounds.width.second)
			- plot()->invTransform(ownBounds.width.first,0)),
// TODO find out what the convention really is
	       h = (ownBounds.height.first == QwtPlot::axisCnt) ? ownBounds.height.second :
			(plot()->invTransform(ownBounds.height.first,0)
			- plot()->invTransform(ownBounds.height.first, ownBounds.height.second)) ;

	// shift x
	switch (anchor)
	{
	case topRight:
	case right:
	case bottomRight:
		px -= w / 2 ;
	case center:
	case top:
	case bottom:
		px -= w / 2 ;
	default: ;
	}

	// shift y
	switch(anchor)
	{
	case bottom:
	case bottomLeft:
	case bottomRight:
		py -= h / 2 ;
	case center:
	case left:
	case right:
		py -= h / 2 ;
	default: ;
	}

	return QRectF(px,py,w,h) ;
}

void specSVGItem::setBoundRect(const QRectF &br)
{
	if (!plot()) return ;
	QPointF anchorPoint = getPoint(anchor,br) ;
	qDebug() << "anchor: " << anchorPoint ;
	ownBounds.width.second = (ownBounds.width.first == QwtPlot::axisCnt) ?  br.width() :
			(plot()->transform(ownBounds.width.first, br.width())
			- plot()->transform(ownBounds.width.first, 0));
	// TODO find out convention
	ownBounds.height.second = (ownBounds.height.first == QwtPlot::axisCnt) ?  br.height() :
			(plot()->transform(ownBounds.height.first, 0)
			- plot()->transform(ownBounds.height.first, br.height()));
	ownBounds.x.second = (ownBounds.x.first == QwtPlot::axisCnt) ? anchorPoint.x() :
			plot()->transform(ownBounds.x.first, anchorPoint.x()) ;
	ownBounds.y.second = (ownBounds.y.first == QwtPlot::axisCnt) ? anchorPoint.y() :
			plot()->transform(ownBounds.y.first, anchorPoint.y()) ;
	qDebug() << "new bounds:" << ownBounds.x.second << ownBounds.y.second << ownBounds.width.second << ownBounds.height.second ;
	highlight(highlighting) ;
}

QPointF specSVGItem::getPoint(SVGCornerPoint point, const QRectF& br) const
{
	switch (point)
	{
	case top:
		return QPointF(br.center().x(),br.top()) ;
	case bottom:
		return QPointF(br.center().x(),br.bottom()) ;
	case left:
		return QPointF(br.left(),br.center().y()) ;
	case right:
		return QPointF(br.right(),br.center().y()) ;
	case topLeft:
		return br.topLeft() ;
	case topRight:
		return br.topRight() ;
	case bottomLeft:
		return br.bottomLeft() ;
	case bottomRight:
		return br.bottomRight() ;
	case center:
		return br.center() ;
	default: ;
	}
	return QPointF() ;
}

void specSVGItem::refreshSVG()
{
	if (!plot()) return ;
	highlight(highlighting) ;
	image.loadData(boundRect(),data) ;
}

void specSVGItem::writeToStream(QDataStream &out) const
{
	specModelItem::writeToStream(out) ;
	out << data << ownBounds << qint8(anchor) ;
}

void specSVGItem::readFromStream(QDataStream &in)
{
	qint8 anc ;
	specModelItem::readFromStream(in) ;
	in >> data >> ownBounds >> anc ;
	anchor = (SVGCornerPoint) anc ;
}

void specSVGItem::pointMoved(const int & p, const double &x, const double &y)
{
	SVGCornerPoint point((SVGCornerPoint) p) ;
	if (point == undefined) return ;
	if (!plot()) return ;
	QRectF br = boundRect() ;
	qDebug() << "old boundingrect:" << br ;
	QPointF newPoint(x,y) ;
	QPointF diff(getPoint(anchor,br)- newPoint) ;
	double aspectRatio = br.width()/br.height() ;
	switch (point)
	{
	case center:	br.moveCenter(newPoint); break ;
	case left:	br.setLeft(x); break ;
	case right:	br.setRight(x); break ;
	case top:	br.setTop(y) ; break ;
	case bottom:	br.setBottom(y) ; break ;
	case topLeft:	br.setTopLeft(newPoint) ; break ;
	case topRight:	br.setTopRight(newPoint) ; break ;
	case bottomLeft:	br.setBottomLeft(newPoint) ; break ;
	case bottomRight:	br.setBottomRight(newPoint) ; break ;
	default: ;
	}
	qDebug() << "new boundingrect:" << br ;
	if (point == topLeft ||
	    point == topRight ||
	    point == bottomLeft ||
	    point == bottomRight)
	{
		if (br.width()/br.height() < aspectRatio)
			br.setWidth(br.height()*aspectRatio);
		else
			br.setHeight(br.width()/aspectRatio) ;
	}
	setBoundRect(br.normalized()) ;
}

specSVGItem::bounds specSVGItem::getBounds() const
{
	return ownBounds ;
}

void specSVGItem::setBounds(const bounds &b)
{
	ownBounds = b ;
}

QDataStream& operator<<(QDataStream& out, const specSVGItem::bounds& b)
{
	return out << b.x << b.y << b.width << b.height ;
}

QDataStream& operator>>(QDataStream& in, specSVGItem::bounds& b)
{
	return in >> b.x >> b.y >> b.width >> b.height ;
}

QIcon specSVGItem::decoration() const
{
	return QIcon::fromTheme("image-x-generic") ;
}
