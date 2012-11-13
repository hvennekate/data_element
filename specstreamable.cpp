#include "specstreamable.h"
#include <QDebug>

specStreamable::type specStreamable::effectiveId() const
{
	return type(typeId()) ;
}

QDataStream& operator<<(QDataStream& out, const specStreamable& item)
{
	QByteArray ba ;
	QDataStream stream(&ba,QIODevice::WriteOnly) ;
	stream << (quint32) item.effectiveId() ;
	item.writeToStream(stream) ;
	out <<  ba ;
//	qDebug() << "Writing item id:" << specStreamable::streamableType(item.effectiveId()) << "Size:" << ba.size() ;
	return out ;
}

QDataStream& operator>>(QDataStream& in, specStreamable& item)
{
	QByteArray ba ;
	in >> ba ;
	QDataStream stream(ba) ;
	quint32 ta ;
	stream >> ta ;
	specStreamable::type t(ta) ;
	if (t != item.effectiveId())
	{
		return in ;
	}
	item.readFromStream(stream) ;
//	qDebug() << "Reading item id:" << specStreamable::streamableType(ta) << "Size:" << ba.size() ;
	return in ;
}

specStreamable *specStreamable::produceItem(QDataStream& in) const
{
	QByteArray ba ;
	in >> ba ;
	QDataStream stream(ba) ;
	quint32 t ;
	stream >> t ;
	specStreamable *item = factory(t) ;
//	qDebug() << "Produced item id:" << t << "Size:" << ba.size() ;
	if (item) item->readFromStream(stream);
	return item ;
}
