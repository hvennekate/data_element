#ifndef SPECWORKERTHREAD_H
#define SPECWORKERTHREAD_H

#include <QThread>
#include <QProgressDialog>

class specWorkerThread : public QThread
{
	Q_OBJECT
protected:
	bool toTerminate ;
	QProgressDialog dialog ;
public:
	explicit specWorkerThread(int maxVal, QObject* parent = 0);
signals:
	void progressValue(int) ;
public slots:
	void finish() ;
};

#endif // SPECWORKERTHREAD_H
