#include "specworkerthread.h"

specWorkerThread::specWorkerThread(int maxVal, QObject *parent) :
	QThread(parent),
	toTerminate(false),
	dialog("Progress","Cancel",0,maxVal)
{
	dialog.setMinimumDuration(10) ;
	connect(this, SIGNAL(progressValue(int)), &dialog, SLOT(setValue(int))) ;
	connect(&dialog, SIGNAL(canceled()), this, SLOT(finish())) ;
}

void specWorkerThread::finish()
{
	toTerminate = true ;
}
