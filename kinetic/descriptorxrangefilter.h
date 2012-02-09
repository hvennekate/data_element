#ifndef DESCRIPTORXRANGEFILTER_H
#define DESCRIPTORXRANGEFILTER_H

#include "specmetafilter.h"

class descriptorXRangeFilter : public specMetaFilter
{
	QString xDescriptor ;

public:
    explicit descriptorXRangeFilter(specMetaItem *parent) ;
	QStringList variables() ;
	QStringList variableValues() ;
	void setVariableValue(const QString &, int index) ;
	QwtSeriesData<QPointF>* data(QList<specModelItem *> items) ;
	QDataStream &write(QDataStream &) const ;
	QDataStream &read(QDataStream &) ;
	void setXDescriptor(const QString&) ;
	QList<specCanvasItem*> plotIndicators() ;
};

#endif // DESCRIPTORXRANGEFILTER_H
