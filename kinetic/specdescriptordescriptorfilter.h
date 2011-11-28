#ifndef SPECDESCRIPTORDESCRIPTORFILTER_H
#define SPECDESCRIPTORDESCRIPTORFILTER_H

#include "specmetafilter.h"

class specDescriptorDescriptorFilter : public specMetaFilter
{
	QString xDescriptor, yDescriptor ;
public:
	explicit specDescriptorDescriptorFilter(specMetaItem *parent) ;
	QStringList variables() ;
	QStringList variableValues() ;
	void setVariableValue(const QString &, int index) ;
	QwtSeriesData<QPointF>* data(QList<specModelItem *> items) ;
	QDataStream &write(QDataStream &) const ;
	QDataStream &read(QDataStream &) ;
	void setXDescriptor(const QString&) ;
	void setYDescriptor(const QString&) ;
};

#endif // SPECDESCRIPTORDESCRIPTORFILTER_H
