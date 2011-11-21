#include "specsvgitem.h"

specSVGItem::specSVGItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  image(0),
	  data(0),
	  highlighting(0)
{
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
	if (!data || !image) return ;
	image->loadData(rect,*data) ;
	highlight(highlighting) ;
}

void specSVGItem::highlight(bool highlight)
{
	highlighting = highlight ;
	if (!image) return ;
	QRectF bounds = image->boundingRect() ;
	QVector<QPointF> points ;
	for (int i = 0 ; i < specSVGItem::size ; ++i)
		points << QPointF() ;
	points[center] = bounds.center() ;
	points[left]   = (bounds.topLeft()+bounds.bottomLeft())/2. ;
	points[right]  = (bounds.topRight()+bounds.bottomRight())/2. ;
	points[top]    = (bounds.topLeft()+bounds.topRight())/2. ;
	points[bottom] = (bounds.bottomLeft()+bounds.bottomRight())/2. ;
	points[topLeft]     = bounds.topLeft() ;
	points[bottomLeft]  = bounds.bottomLeft() ;
	points[topRight]    = bounds.topRight() ;
	points[bottomRight] = bounds.bottomRight() ;
	qDebug() << "bounding points:" << points ;
	setSamples(points) ;
	if (!highlight)
		setSymbol(0) ;
	else
		setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::black),QPen(Qt::white),QSize(5,5))) ;
}
