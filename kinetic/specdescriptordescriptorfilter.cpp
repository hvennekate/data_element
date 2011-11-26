#include "specdescriptordescriptorfilter.h"

specDescriptorDescriptorFilter::specDescriptorDescriptorFilter(specMetaItem *parent)
	:specMetaFilter(parent)
{
}

QStringList specDescriptorDescriptorFilter::variables()
{
	return QStringList() << "x" << "y" ;
}

void specDescriptorDescriptorFilter::setXDescriptor(const QString &desc)
{
	if (parent) parent->invalidate();
	xDescriptor = desc ;
}

void specDescriptorDescriptorFilter::setYDescriptor(const QString &)
{
	if (parent) parent->invalidate();
	yDescriptor = desc ;
}

QStringList specDescriptorDescriptorFilter::variableValues()
{
	return QStringList() << xDescriptor << yDescriptor ;
}

void specDescriptorDescriptorFilter::setVariableValue(const QString &desc, int index)
{
	if (index == 0)
		setXDescriptor(desc) ;
	else if (index==1)
		setYDescriptor(desc) ;
}

QDataStream &specDescriptorDescriptorFilter::write(QDataStream &out) const
{
	return out ; // TODO
}

QDataStream &specDescriptorDescriptorFilter::read(QDataStream &in)
{
	return in ; // TODO
}

QwtSeriesData<QPointF>* specDescriptorDescriptorFilter::data(QList<specModelItem *> items)
{
	QVector<QPointF> series ;
	foreach(specModelItem* item, items)
		series << QPointF(item->descriptor(xDescriptor).toDouble(),
						  item->descriptor(yDescriptor).toDouble()) ; // TODO maybe check for numeric property
	return new QwtPointSeriesData(series) ;
}
