#ifndef SPECINSTREAM_H
#define SPECINSTREAM_H

#include "specstream.h"

class specInStream : public specStream
{
private:
	Type currentType ;
	bool produceArray() ;
	void close() {}
public:
	explicit specInStream(QDataStream*) ;
	explicit specInStream(QByteArray*) ;
	bool next() ;
	Type type() ;
	void skip() ;
	bool toNext(Type) ; // QVector<Type>
	bool expect(Type) ;

	template<class T> specInStream& operator>>(T& object)
	{
		*currentStream >> object ;
		return (*this) ;
	}
};

#endif // SPECINSTREAM_H
