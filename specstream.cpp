#include "specstream.h"

specStream::specStream(QByteArray *ba, QIODevice::OpenModeFlag mode)
	: byteArray(ba),
	  currentArray(0),
	  stream(new QDataStream(ba, mode)),
	  currentStream(0)
{}

specStream::specStream(QDataStream *dstr)
	: byteArray(0),
	  currentArray(0),
	  stream(dstr),
	  currentStream(0)
{}

specStream::~specStream()
{
	delete currentArray ;
	delete currentStream ;
	if (byteArray) delete stream ;
}
