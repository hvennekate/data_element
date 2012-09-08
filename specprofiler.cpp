#include "specprofiler.h"
#include <QDebug>

specProfiler::specProfiler(const QString &s)
	: description(s)
{
	start();
}

specProfiler::~specProfiler()
{
	qDebug() << description << elapsed() ;
}
