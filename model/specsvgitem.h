#ifndef SPECSVGITEM_H
#define SPECSVGITEM_H

#include "specmodelitem.h"
#include <qwt_plot_svgitem.h>
#include <QRectF>

class specSVGItem : public specModelItem
{
private:
	QwtPlotSvgItem *image ;
protected:
	QDataStream& readFromStream(QDataStream &in){ return in ; }
	QDataStream& writeToStream(QDataStream &out) const { return out ; }
public:
	specSVGItem(specFolderItem* par=0, QString description="") ;
	int rtti() const { return spec::SVGItem ; }
	void attach(QwtPlot *plot) ;
	void setImage(const QRectF&, const QByteArray&) ;
};

#endif // SPECSVGITEM_H
