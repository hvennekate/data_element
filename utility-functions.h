#ifndef SPECUTILITIES_H
#define SPECUTILITIES_H

#ifdef DOUBLEDEVIATIONCORRECTION
#include <cfloat>
#endif

#include <QList>
#include <QVector>
#include <QHash>
#include <QString>
#include <QStringList>
#include "specdataitem.h"
#include <QTextStream>
#include <QFile>
#include <names.h>

// bool roundedEqual(double a, double b) ;
QList<specModelItem*> readLogFile(QFile& file) ;
QList<specModelItem*> readHVFile(QFile&) ;
QList<specModelItem*> readPEFile(QFile&) ;
QList<specModelItem*> readPEBinary(QFile&) ;
QList<specModelItem*> readJCAMPFile(QFile&) ;
QList<specModelItem*> readSKHIFile(QFile&) ;
QList<specModelItem*> readXYFILE(QFile&) ;
QPair<QString,specDescriptor> readJCAMPldr(QString &first,QTextStream &in) ;
specModelItem* readJCAMPBlock(QTextStream& in) ;
void readJCAMPdata(QTextStream& in, QVector<specDataPoint>& data, double step, double xfactor, double yfactor) ;
QVector<double> waveNumbers(QTextStream&) ;
QHash<QString,specDescriptor> fileHeader(QTextStream&) ;
specFileImportFunction fileFilter(const QString& fileName);
QVector<double> gaussjinv(QVector<QVector<double> >&, QVector<double>&) ;

bool comparePoints(const QPointF&, const QPointF&) ;

/*! Accumulate items equal to begin and average them in return value.
    begin will be changed to past the last accumulated item.*/
template<typename T, class forwardIterator, typename binaryPredicate> // TODO mehr benutzen!
inline T average(forwardIterator& begin,const forwardIterator& end, binaryPredicate equal, const T first)
{
	begin++ ;
	T retVal = first ;
	int count = 1 ;
	while (begin != end && equal(*begin,first))
	{
		retVal += *begin++ ;
		++ count ;
	}
	return retVal /= (double) count ;
}

//template<class T, class forwardIterator> // TODO mehr benutzen!
//inline T average(forwardIterator& begin,const forwardIterator& end)
//{
//	const T first = *begin++ ;
//	T retVal = first ;
//	int count = 1 ;
//	while (begin != end && *begin == first)
//	{
//		retVal += *begin++ ;
//		++ count ;
//	}
//	return retVal /= (double) count ;
//}

template<class forwardIterator, class outputIterator, typename binaryPredicate>
inline void averageToNew(forwardIterator begin, forwardIterator end,
			 binaryPredicate equal, outputIterator target)
{
	while (begin != end)
		*target++ = average(begin, end, equal, *begin) ;
}

//template<class forwardIterator, class outputIterator>
//inline void averageToNew(forwardIterator begin, forwardIterator end,
//		  outputIterator target)
//{
//	while (begin != end)
//		*target++ = average(begin, end) ;
//}

inline bool doubleComparison(const double& a, const double& b)
{
	// this is an evil hack to overcome double precision's limitations...
#ifdef DOUBLEDEVIATIONCORRECTION
	return fabs(a - b) <= 2 * DBL_EPSILON * qMax(qMax(1.0, fabs(a)), fabs(b)) ;
#else
	return a == b ;
#endif
}

#endif
