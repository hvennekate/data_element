#include "specsvgitem.h"
#include "specplot.h"
#include "canvaspicker.h"
#include <QSvgRenderer>
#include "svgitemproperties.h"

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

	specPlot *sp = qobject_cast<specPlot*> (plot()) ; // TODO make function (also specMetaRange)
	if (sp && sp->svgPicker())
	{
		if (highlight) sp->svgPicker()->addSelectable(this) ;
		else sp->svgPicker()->removeSelectable(this) ;
	}
}

specSVGItem::specSVGItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  highlighting(false),
	  anchor(topLeft)
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
	anchor = center ;
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
	case top:
	case topLeft:
	case topRight:
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
	highlight(highlighting) ;
}

QPointF specSVGItem::getPoint(SVGCornerPoint point, const QRectF& br) const
{
	switch (point)
	{
	case top:
		return QPointF(br.center().x(),br.bottom()) ;
	case bottom:
		return QPointF(br.center().x(),br.top()) ;
	case left:
		return QPointF(br.left(),br.center().y()) ;
	case right:
		return QPointF(br.right(),br.center().y()) ;
	case topLeft:
		return br.bottomLeft() ;
	case topRight:
		return br.bottomRight() ;
	case bottomLeft:
		return br.topLeft() ;
	case bottomRight:
		return br.topRight() ;
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
	QPointF newPoint(x,y) ;

	if ((point == topLeft ||
	    point == topRight ||
	    point == bottomLeft ||
	    point == bottomRight) &&
		br.width() && br.height())
	{
		QPointF oldPoint(getPoint(point,br)) ;
		QPointF diff(newPoint - oldPoint) ;
		if (point == bottomRight || point == topLeft)
			diff.setY(-diff.y()) ;
		if (br.height()*diff.x() < br.width()*diff.y())
			diff.setX(diff.y()*br.width()/br.height());
		else
			diff.setY(diff.x()*br.height()/br.width());
		if (point == bottomRight || point == topLeft)
			diff.setY(-diff.y()) ;
		newPoint = oldPoint + diff ;
	}

	switch (point)
	{
	case center:	br.moveCenter(newPoint); break ;
	case left:	br.setLeft(x); break ;
	case right:	br.setRight(x); break ;
	case top:	br.setBottom(y) ; break ;
	case bottom:	br.setTop(y) ; break ;
	case topLeft:	br.setBottomLeft(newPoint) ; break ;
	case topRight:	br.setBottomRight(newPoint) ; break ;
	case bottomLeft:	br.setTopLeft(newPoint) ; break ;
	case bottomRight:	br.setTopRight(newPoint) ; break ;
	default: ;
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

specUndoCommand* specSVGItem::itemPropertiesAction(QObject *parentObject)
{
	svgItemProperties propertiesDialog(this) ;
	if (propertiesDialog.exec() != QDialog::Accepted) return 0 ;
	return propertiesDialog.generateCommand(parentObject) ;
}

specSVGItem::SVGCornerPoint specSVGItem::setAnchor(const SVGCornerPoint & p)
{
	SVGCornerPoint old = anchor ;
	anchor = p ;
	return old ;
}

bool specSVGItem::bounds::operator ==(const specSVGItem::bounds& other) const
{
	return x      == other.x     &&
	       y      == other.y     &&
	       width  == other.width &&
	       height == other.height ;
}
