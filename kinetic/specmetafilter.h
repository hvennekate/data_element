#ifndef SPECMETAFILTER_H
#define SPECMETAFILTER_H

#include "specmetaitem.h"
#include "speccanvasitem.h"
#include "qwt/qwt_series_data.h"

class specMetaFilter
{
private:
	specMetaItem *parent ;
public:
	explicit specMetaFilter(specMetaItem* parent = 0) ;
	void setParent(specMetaItem *parent) ;
	virtual QStringList variables() { return QStringList() ; }
	virtual QStringList variableValues() { return QVector<double>() ; }
	virtual void setVariableValue(const QString&, int index) { ; }
	virtual QwtSeriesData<QPointF>* data(QList<specModelItem*> items) = 0 ;
	virtual QList<specCanvasItem*> plotIndicators() { return QList<specCanvasItem*>() ; }
	virtual QDataStream& write(QDataStream&) const = 0 ;
	virtual QDataStream& read(QDataStream&) = 0 ;
};

#endif // SPECMETAFILTER_H
