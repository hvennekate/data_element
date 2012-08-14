#ifndef SPECSVGITEM_H
#define SPECSVGITEM_H

#include "specmodelitem.h"
#include <qwt_plot_svgitem.h>
#include <QRectF>

class specSVGItem : public specModelItem
{
	enum SVGCornerPoints : qint8 { undefined = -1,
		center      = 0,
		left        = 1,
		right       = 2,
		top         = 3,
		bottom      = 4,
		topLeft     = 5,
		bottomLeft  = 6,
		topRight    = 7,
		bottomRight = 8,
		size        = 9 } ;
	typedef QPair<qint8,qreal> dimension ;
	QwtPlotSvgItem image ;
	QByteArray data ;
	bool highlighting ;
	dimension x, y, width, height ;
	QPair<QBool,qreal> aspect ;
	SVGCornerPoints anchor ;
	void redrawImage() ;
	double transform(const dimension&) ;

	QPointF anchorPoint(const SVGCornerPoints& point) const;
	void setAnchor(QRectF &bounds, const QPointF&, const SVGCornerPoints& point) const ;
	type typeId() const { return specStreamable::svgItem ; }
	void readFromStream(QDataStream &in) ;
	void writeToStream(QDataStream &out) const;
public:
	specSVGItem(specFolderItem* par=0, QString description="") ;
	int rtti() const { return spec::SVGItem ; }
	void attach(QwtPlot *plot) ;
	void setImage(const QByteArray&) ;
	void setBoundingRect(const QRectF&) ;
	void highlight(bool highlight) ;
	void refreshSVG(double,double);
};

#endif // SPECSVGITEM_H
