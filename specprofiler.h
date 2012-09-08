#ifndef SPECPROFILER_H
#define SPECPROFILER_H

#include <QTime>

#ifdef QT_DEBUG
#define SPECPROFILEMACRO(STRING) profiler DUMMYPROFILER(STRING) ;
#else
#define SPECPROFILEMACRO(STRING)
#endif

class specProfiler : public QTime
{
private:
	QString description ;
public:
    specProfiler(const QString&) ;
    ~specProfiler() ;
};

#endif // SPECPROFILER_H
