#include "specoutstream.h"

specOutStream::specOutStream(QByteArray *ba)
	: specStream(ba, QIODevice::WriteOnly)
{
	close() ;
}

specOutStream::specOutStream(QDataStream *dstr)
	: specStream(dstr)
{
	close() ;
}

specOutStream::~specOutStream()
{
	while (types.size())
	{
		types.top() = 0 ;
		close() ;
	}
}

void specOutStream::close()
{
	if (!types.isEmpty())
	{
		delete currentStream ;
		if (stream && currentArray)
			*stream << *currentArray ;
		delete currentArray ;
		if (isContainer(types.top())) types.top() = 0;
		else types.pop() ;
	}
	currentArray = new QByteArray ;
	currentStream = new QDataStream(currentArray, QIODevice::WriteOnly) ;
}

bool specOutStream::next(const Type& t)
{
	if (!currentArray->isEmpty())
		close() ;
	*currentStream << t ;
	types << t ;
	return true ;
}

void specOutStream::startContainer(const Type &t)
{
	next(t | containerType) ;
}

void specOutStream::stopContainer()
{
	close() ; // close last item
	close() ; // close 0-item of container
}
