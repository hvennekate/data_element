#include "bzipiodevice.h"
#include <bzlib.h>

struct bzipIODevicePrivate : bz_stream
{
	bzipIODevicePrivate()
	{
		bzalloc = 0 ;
		bzfree  = 0 ;
		opaque  = 0 ;
	}
	qint64 outPos() const
	{
		return ((qint64) total_out_hi32 << 32) + total_out_lo32 ;
	}
	qint64 inPos() const
	{
		return ((qint64) total_in_hi32 << 32) + total_in_lo32 ;
	}
};

bzipIODevice::bzipIODevice(QIODevice* inputDevice, QObject* parent) :
	QIODevice(parent),
	internalDevice(inputDevice),
	zipStream(new bzipIODevicePrivate),
	buffer(1, 0),
	lastPos(0)
{
}

bzipIODevice::~bzipIODevice()
{
	close() ;
	delete internalDevice ;
	delete zipStream ;
}

bool bzipIODevice::open(OpenMode mode)
{
	if(isOpen()) return false ;
	if(!internalDevice) return false ;
	if(!QIODevice::open(mode)) return false ;
	if(internalDevice->openMode() != mode && !internalDevice->open(mode)) return false ;   // TODO maybe try closing...
	lastPos = 0 ;
	if(mode == ReadOnly)
	{
		if(BZ2_bzDecompressInit(zipStream, 0, 0) != BZ_OK) return false ;
		zipStream->avail_in = 0 ;
		zipStream->next_in = 0 ;
		return true ;
	}
	if(mode == WriteOnly) return BZ2_bzCompressInit(zipStream, 9, 0, 0) == BZ_OK ;
	return false ;
}

bool bzipIODevice::bufferToDevice()
{
	if(buffer.size() == internalDevice->write(buffer))
	{
		buffer.clear();
		return true ;
	}
	return false ;
}

void bzipIODevice::close()
{
	if(!isOpen()) return ;
	if(openMode() == ReadOnly) BZ2_bzDecompressEnd(zipStream) ;
	if(openMode() == WriteOnly)
	{
		prepareInBuffer(0) ;
		while(BZ2_bzCompress(zipStream, BZ_FINISH) != BZ_STREAM_END)
		{
			writeBuffer() ;
			prepareInBuffer(0);
		}
		writeBuffer() ;
		BZ2_bzCompressEnd(zipStream) ;
	}
	QIODevice::close() ;
	internalDevice->close() ;
}

void bzipIODevice::prepareInBuffer(int minsize)
{
	buffer.resize(qMax(buffer.size(), minsize)) ;
	zipStream->next_out = buffer.data() ;
	zipStream->avail_out = buffer.size() ;
	lastPos = zipStream->outPos() ;
}

void bzipIODevice::fillOutBuffer(int minsize)
{
	buffer = internalDevice->read(minsize) ;
	zipStream->next_in = buffer.data() ;
	zipStream->avail_in = buffer.size() ;
}

bool bzipIODevice::writeBuffer()
{
	qint64 bytes = zipStream->outPos() - lastPos ;
	if(internalDevice->write(buffer.data(), bytes) != bytes) return false ;
	lastPos += bytes ;
	return true ;
}

bool bzipIODevice::atEnd() const
{
	if(internalDevice && zipStream && zipStream->state == BZ_RUN && errorString().isEmpty()) return false;  // TODO improve
	return true ;
}

bool bzipIODevice::isSequential() const
{
	return true ;
}

qint64 bzipIODevice::writeData(const char* data, qint64 len)
{
	if(!len) return 0 ;
	if(!internalDevice) return -1 ;
	if(!isOpen()) return -1 ;
	if(openMode() != WriteOnly) return -1 ;
	zipStream->next_in = const_cast<char*>(data) ;  // TODO this is a design deficit in bz2lib...
	zipStream->avail_in = len ;

	while(zipStream->avail_in)
	{
		prepareInBuffer(len) ;
		int rv = BZ2_bzCompress(zipStream, BZ_RUN) ;
		if(rv != BZ_OK && rv != BZ_RUN_OK) break ;  // TODO consider rv < 0
		if(!writeBuffer()) return -1 ;  // TODO
	}
	return len ;
}

qint64 bzipIODevice::readData(char* data, qint64 maxlen)
{
	if(!maxlen) return 0 ;
	if(!internalDevice) return -1 ;
	if(!isOpen()) return -1 ;
	if(openMode() != ReadOnly) return -1 ;
	zipStream->next_out = data ;
	zipStream->avail_out = maxlen ;

	while(zipStream->avail_out)
	{
		if(!zipStream->avail_in)
		{
			fillOutBuffer(maxlen) ;
			if(!zipStream->avail_in) break ;
		}
		int rv = BZ2_bzDecompress(zipStream) ;
		if(rv == BZ_STREAM_END) break ;
		if(rv != BZ_OK) return -1 ;  // TODO
	}

	qint64 written = zipStream->outPos() - lastPos ;
	lastPos += written ;
	return written ;
}


QIODevice* bzipIODevice::releaseDevice()
{
	close() ;
	QIODevice* dev = internalDevice ;
	internalDevice = 0 ;
	return dev ;
}
