#include "specinstream.h"

specInStream::specInStream(QByteArray *ba)
	: specStream(ba, QIODevice::ReadOnly)
{}

specInStream::specInStream(QDataStream *dstr)
	: specStream(dstr)
{}

bool specInStream::produceArray()
{
	delete currentStream ;
	delete currentArray ;
	currentStream = 0 ;
	currentArray = 0 ;
	currentArray = new QByteArray ;
	if (stream->atEnd() || stream->status())
		return false ;
	*stream >> *currentArray ;
	return true ;
}

bool specInStream::next()
{
	if (!produceArray()) return false ;
	if (currentArray->isEmpty())
		return false ;
	currentStream = new QDataStream(currentArray,QIODevice::ReadOnly) ;
	if (currentArray->isEmpty())
		currentType = 0 ;
	else
		*currentStream >> currentType ;
	return true ;
}

bool specInStream::expect(Type t)
{
	return (next() && type() == t) ;
}

specStream::Type specInStream::type()
{
	return currentType & ~(specStream::containerType)  ;
}

void specInStream::skip()
{
	if (isContainer(currentType))
		while (!currentArray->isEmpty())
			produceArray() ;
}

bool specInStream::toNext(Type t)
{
	while (next())
	{
		if (type() == t) return true ;
		skip() ;
	}
	return false ;
}
