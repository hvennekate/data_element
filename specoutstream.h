#ifndef SPECOUTSTREAM_H
#define SPECOUTSTREAM_H
#include "specstream.h"

class specOutStream : public specStream
{
private:
	void close() ;
	QStack<Type> types ;
public:
	explicit specOutStream(QDataStream*) ;
	explicit specOutStream(QByteArray*) ;
	~specOutStream() ;


	bool next(const Type& t) ;
	void startContainer(const Type& t) ;
	void stopContainer() ;

	template<class T> specOutStream& operator<<(const T& object)
	{
		*currentStream << object ;
		return (*this) ;
	}
};

#endif // SPECOUTSTREAM_H
