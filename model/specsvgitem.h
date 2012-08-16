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
	SVGCornerPoint anchor ;

	double transform(const dimension&) const ;
	void setDimension(dimension&, double) ;

	type typeId() const { return specStreamable::svgItem ; }
	void readFromStream(QDataStream &in) ;
	void writeToStream(QDataStream &out) const;
	QRectF boundRect() const ;
	QPointF getPoint(SVGCornerPoint, const QRectF&) const ;
	void setBoundRect(const QRectF&) ;
public:
	specSVGItem(specFolderItem* par=0, QString description="") ;
	int rtti() const { return spec::SVGItem ; }
	void attach(QwtPlot *plot) ;
	void setImage(const QByteArray&) ;
	void highlight(bool highlight) ;
	void refreshSVG();
	void pointMoved(const int &, const double &, const double &);
	struct bounds
	{
		dimension x, y, width, height ;
	};
	bounds getBounds() const ;
	void setBounds(const bounds&) ;
	QIcon decoration() const ;
private:
	bounds ownBounds ;
};

QDataStream& operator<<(QDataStream& out, const specSVGItem::bounds& b) ;
QDataStream& operator>>(QDataStream& in, specSVGItem::bounds& b) ;

#endif // SPECSVGITEM_H
