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
	if (getParent()) getParent()->invalidate();
	xDescriptor = desc ;
}

void specDescriptorDescriptorFilter::setYDescriptor(const QString &desc)
{
	if (getParent()) getParent()->invalidate();
	yDescriptor = desc ;
}

QStringList specDescriptorDescriptorFilter::variableValues()
{
	qDebug() << "called variable values" << xDescriptor << yDescriptor ;
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
	qDebug() << "Data in descriptor-filter:" << series ;
	qSort(series.begin(),series.end(),specMetaFilter::lessThanXValue) ;
	return new QwtPointSeriesData(series) ;
}
