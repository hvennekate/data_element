#ifndef SPECSVGITEM_H
#define SPECSVGITEM_H

#include "specmodelitem.h"
#include <qwt_plot_svgitem.h>
#include <QRectF>
#include <QPointF>

class specSVGItem : public specModelItem
{
public:
	typedef QPair<qint8,qreal> dimension ;
private:
	enum SVGCornerPoint { undefined = -1,
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

	QwtPlotSvgItem image ;
	QByteArray data ;
	bool highlighting ;
	QBool keepAspect ;
	SVGCornerPoint anchor ;

	void redrawImage() ;
	double transform(const dimension&) ;
	void setDimension(dimension&, double) ;

	type typeId() const { return specStreamable::svgItem ; }
	void readFromStream(QDataStream &in) ;
	void writeToStream(QDataStream &out) const;
	QRectF boundRect() const ;
	QPointF getPoint(SVGCornerPoint) const ;
	double resizeDimension(double dim,
					    double delta,
					    const QVector<SVGCornerPoint>& lower,
					    const QVector<SVGCornerPoint>& upper,
					    SVGCornerPoint point) ;
public:
	specSVGItem(specFolderItem* par=0, QString description="") ;
	int rtti() const { return spec::SVGItem ; }
	void attach(QwtPlot *plot) ;
	void setImage(const QByteArray&) ;
	void highlight(bool highlight) ;
	void refreshSVG(double,double);
	void pointMoved(const int &, const double &, const double &);
	bool setKeepAspectRatio(bool keep) ;
	struct bounds
	{
		dimension x, y, width, height ;
	};
	bounds getBounds() const ;
	void setBounds(const bounds&) ;
private:
	bounds ownBounds ;
};

QDataStream& operator<<(QDataStream& out, const specSVGItem::bounds& b)
{
	return out << b.x << b.y << b.width << b.height ;
}

QDataStream& operator>>(QDataStream& in, const specSVGItem::bounds& b)
{
	return in >> b.x >> b.y >> b.width >> b.height ;
}

#endif // SPECSVGITEM_H
