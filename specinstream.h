#ifndef SPECINSTREAM_H
#define SPECINSTREAM_H

#include "specstream.h"

class specInStream : public specStream
{
private:
	Type currentType ;
	bool produceArray() ;
	void close() {}
	void skip() ;
public:
	explicit specInStream(QDataStream*) ;
	explicit specInStream(QByteArray*) ;
	bool next() ;
	Type type() ;
	bool toNext(Type) ; // QVector<Type>
	bool expect(Type) ;
	bool expect(const QVector<Type>&) ;

	template<class T> specInStream& operator>>(T& object)
	{
		*currentStream >> object ;
		return (*this) ;
	}
};

#endif // SPECINSTREAM_H
