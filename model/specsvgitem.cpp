#include "specsvgitem.h"
#include <qwt/qwt_plot.h>
#include <QSvgRenderer>

void specSVGItem::attach(QwtPlot *plot)
{
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

void specSVGItem::setDimension(dimension &dim, double val)
{
	dim.second = plot() ? plot()->transform(dim.first,val) : val ;
}

specSVGItem::specSVGItem(specFolderItem *par, QString description, double w, double h, double xp, double yp)
	: specModelItem(par,description),
	  highlighting(0),
	  anchor(center),
	  width(qMakePair(QwtPlot::axisCnt,0)),
	  height(qMakePair(QwtPlot::axisCnt,0)),
	  x(qMakePair(QwtPlot::axisCnt,0)),
	  y(qMakePair(QwtPlot::axisCnt,0)),
	  keepAspect(true)
{
	setStyle(QwtPlotCurve::NoCurve);
}

void specSVGItem::setImage(const QByteArray &newData)
{
	if (data.isEmpty())
	{
		QSize defaultSize(QSvgRenderer(newData).defaultSize()) ;
		width = qMakePair(QwtPlot::xBottom,defaultSize.width()) ;
		height = qMakePair(QwtPlot::yLeft, defaultSize.height()) ;
	}
	data = newData ;
//	redrawImage();
}

QRectF specSVGItem::boundRect() const
{
	double px = transform(ownBounds.x),
			py = transform(ownBounds.y),
			w = transform(ownBounds.width),
			h = transform(ownBounds.height) ;

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
	default:
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
	default:
	}

	return QRectF(px,py,w,h) ;
}

QPointF specSVGItem::getPoint(SVGCornerPoint point) const
{
	QRectF br = boundRect() ;
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
	default:
	}
	return QPointF() ;
}

void specSVGItem::redrawImage()
{
	if (!plot()) return ;
	image.loadData(boundRect(),data) ;
}

void specSVGItem::refreshSVG(double xfactor, double yfactor)
{
	if (!plot()) return ;
	if (!image) return ;
//	QPointF fixPoint = anchorPoint(fix) ;
//	QRectF bounds = image->boundingRect() ;
//	if (width  >= 0) bounds.setWidth(width * xfactor) ;
//	if (height >= 0) bounds.setHeight(height * yfactor) ;
//	if (fix >= 0)    setAnchor(bounds, fixPoint, fix) ;
//	setBoundingRect(bounds) ;
	redrawImage();
}

void specSVGItem::writeToStream(QDataStream &out) const
{
	specModelItem::writeToStream(out) ;
	out << data << ownBounds << keepAspect << qint8(anchor) ;
}

void specSVGItem::readFromStream(QDataStream &in)
{
	qint8 anc ;
	specModelItem::readFromStream(in) ;
	in >> data >> ownBounds >> keepAspect >> anc ;
	anchor = anc ;
}

void specSVGItem::resizeDimension(dimension& dim,
				    double delta,
				    const QVector<SVGCornerPoint>& lower,
				    const QVector<SVGCornerPoint>& upper,
				    SVGCornerPoint point)
{
	if (point == undefined) return ;
	int movedCoeff = (lower.contains(point) ? -1 :
				(upper.contains(point) ? 1 : 0)) ;
	int refCoeff = (lower.contains(anchor) ? -1 :
				(upper.contains(anchor) ? 1 : 0)) ;
	if (movedCoeff == refCoeff) return ;
	setDimension(dim, transform(dim)+ 2./(movedCoeff - refCoeff) *delta) ;
}

void specSVGItem::pointMoved(const int & p, const double &x, const double &y)
{
	SVGCornerPoint point(p) ;
	if (point == undefined) return ;
	if (!plot()) return ;
	if (point == anchor)
	{
		QPointF diff(getPoint(anchor)- QPoint(x,y)) ;
		setDimension(x,transform(x)+diff.x());
		setDimension(y,transform(x)+diff.y());
		return ;
	}
	QVector<SVGCornerPoint> htop, hmiddle, hbottom, wleft, wmiddle, wright ;
	htop << top << topLeft << topRight ;
	hbottom << bottom << bottomLeft << bottomRight ;
//	hmiddle << center << left << right ;

	wleft << left << topLeft << bottomLeft ;
//	wmiddle << top << center << bottom ;
	wright  << right << topRight << bottomRight ;

	resizeDimension(ownBounds.width, )
}

bool specSVGItem::setKeepAspectRatio(bool keep)
{
	qSwap(keep,keepAspect) ;
	return keep ;
}

specSVGItem::bounds specSVGItem::getBounds() const
{
	return ownBounds ;
}

void specSVGItem::setBounds(const bounds &b)
{
	ownBounds = b ;
}
