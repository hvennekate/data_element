#include "specsvgitem.h"

specSVGItem::specSVGItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  image(0)
{
}

void specSVGItem::setImage(const QRectF &rect, const QByteArray &data)
{
	if (!image)
		image = new QwtPlotSvgItem ;
	image->loadData(rect,data) ;
}

void specSVGItem::attach(QwtPlot *plot)
{
	if (image)
		image->attach(plot) ;
}
