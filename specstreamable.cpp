#include "specstreamable.h"

specStreamable::type specStreamable::effectiveId() const
{
	return type(typeId()) ;
}

QDataStream& operator<< (QDataStream& out, const specStreamable& item)
{
	QByteArray ba ;
	QDataStream stream(&ba, QIODevice::WriteOnly) ;
	stream << (quint32) item.effectiveId() ;
	item.writeToStream(stream) ;
	out <<  ba ;
	return out ;
}

QDataStream& operator>> (QDataStream& in, specStreamable& item)
{
	QByteArray ba ;
	in >> ba ;
	QDataStream stream(ba) ;
	quint32 ta ;
	stream >> ta ;
	specStreamable::type t(ta) ;
	if(t != item.effectiveId())
	{
		return in ;
	}
	item.readFromStream(stream) ;
	return in ;
}

specStreamable* specStreamable::produceItem(QDataStream& in) const
{
	QByteArray ba ;
	in >> ba ;
	QDataStream stream(ba) ;
	quint32 t ;
	stream >> t ;
	specStreamable* item = factory(t) ;
	if(item) item->readFromStream(stream);
	return item ;
}
