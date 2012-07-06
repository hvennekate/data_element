#ifndef SPECSTREAM_H
#define SPECSTREAM_H

#include <QDataStream>
#include <QByteArray>
#include <QStack>

class specStream
{
public:
	typedef quint8 Type ;
	const static Type containerType = 128 ;
	virtual void close() = 0 ;
protected:
	QByteArray *byteArray, *currentArray ;
	QDataStream *stream, *currentStream ;
	static inline bool isContainer(const Type& t) { return t & containerType ; }
public:
	explicit specStream(QDataStream*) ;
	explicit specStream(QByteArray*, QIODevice::OpenModeFlag) ;
	virtual ~specStream() ;
};

#endif // SPECSTREAM_H
