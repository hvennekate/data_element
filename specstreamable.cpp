#include "specstreamable.h"
#include <QDebug>

specStreamable::type specStreamable::effectiveId() const
{
	return type(typeId() | type(isContainer() ? type(specStreamable::container) : type(0))) ;
}

QDataStream& operator<<(QDataStream& out, const specStreamable& item)
{
	QByteArray ba ;
	QDataStream stream(&ba,QIODevice::WriteOnly) ;
	stream << item.effectiveId() ;
	item.writeToStream(stream) ;
	out << ba ;
	if (item.isContainer())
	{
		item.writeContents(out);
		out << QByteArray() ;
	}
	return out ;
}

QDataStream& operator>>(QDataStream& in, specStreamable& item)
{
	QByteArray ba ;
	in >> ba ;
	QDataStream stream(ba) ;
	specStreamable::type t ;
	stream >> t ;
	if (t != item.effectiveId())
	{
		if (specStreamable::isContainer(t))
			specStreamable::skipContainer(in) ;
		return in ;
	}
	item.readFromStream(stream) ;
	if (item.isContainer())
	{
		item.readContents(in) ;
		specStreamable::skipContainer(in) ;
	}
	return in ;
}

void specStreamable::skipContainer(QDataStream& in)
{ // invariant:  we have entered the container and are expecting the container termination (empty QByteArray)
	int containerCount = 1 ;
	while (containerCount)
	{
		if (in.status()) return ; // too bad, no more input.
		QByteArray ba ;
		in >> ba ;
		if (ba.isEmpty()) --containerCount ; // Yesss, we've reached the end of a container
		QDataStream stream(ba) ;
		type t = 0 ;
		stream >> t ;
		if (isContainer(t)) ++containerCount; // Ooops, stumbled over another (nested) container to skip
	}
}

specStreamable *specStreamable::produceItem(QDataStream& in) const
{
	QByteArray ba ;
	in >> ba ;
	QDataStream stream(ba) ;
	type t ;
	stream >> t ;
	specStreamable *item = factory(t) ;
	if (item) item->readFromStream(stream);
	return item ;
}
