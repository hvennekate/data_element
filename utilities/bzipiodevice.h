#ifndef BZIPIODEVICE_H
#define BZIPIODEVICE_H

#include <QIODevice>

struct bzipIODevicePrivate ;

class bzipIODevice : public QIODevice
{
	Q_OBJECT
private:
	QIODevice *internalDevice ;
	bzipIODevicePrivate *zipStream ;
	QByteArray buffer ;
	qint64 lastPos ;
	qint64 readData(char *data, qint64 maxlen) ;
	qint64 writeData(const char *data, qint64 len) ;
	bool bufferToDevice() ;
	void prepareInBuffer(int minsize) ;
	void fillOutBuffer(int minsize) ;
	bool writeBuffer() ;
public:
	explicit bzipIODevice(QIODevice *inputDevice= 0, QObject *parent = 0);
	~bzipIODevice() ; //
	QIODevice *releaseDevice() ;
	bool atEnd() const ; //
//	qint64 bytesAvailable() const ;
//	qint64 bytesToWrite() const ;
	void close() ; //
	bool isSequential() const ; //
	bool open(OpenMode mode) ; //
//	bool reset() ;
//	bool seek(qint64 pos) ;
};

#endif // BZIPIODEVICE_H
