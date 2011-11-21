#ifndef SPECSVGITEM_H
#define SPECSVGITEM_H

#include "specmodelitem.h"
#include <qwt_plot_svgitem.h>
#include <QRectF>

class specSVGItem : public specModelItem
{
private:
	QwtPlotSvgItem *image ;
	QByteArray *data ;
	bool highlighting ;
protected:
	QDataStream& readFromStream(QDataStream &in){ return in ; }
	QDataStream& writeToStream(QDataStream &out) const { return out ; }
public:

	enum SVGCornerPoints { center      = 0,
						   left        = 1,
						   right       = 2,
						   top         = 3,
						   bottom      = 4,
						   topLeft     = 5,
						   bottomLeft  = 6,
						   topRight    = 7,
						   bottomRight = 8,
						   size        = 9 } ;

	specSVGItem(specFolderItem* par=0, QString description="") ;
	int rtti() const { return spec::SVGItem ; }
	void attach(QwtPlot *plot) ;
	void setImage(const QRectF&, const QByteArray&) ;
	void setBoundingRect(const QRectF&) ;
	void highlight(bool highlight) ;
};

#endif // SPECSVGITEM_H
