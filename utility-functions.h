#ifndef SPECUTILITIES_H
#define SPECUTILITIES_H

#include <QList>
#include <QVector>
#include <QHash>
#include <QString>
#include <QStringList>
#include "specdataitem.h"
#include <QTextStream>
#include <QFile>

// bool roundedEqual(double a, double b) ;
QList<specModelItem*> readLogFile(QFile& file) ;
QList<specModelItem*> readHVFile(QFile&) ;
QList<specModelItem*> readPEFile(QFile&) ;
QList<specModelItem*> readJCAMPFile(QFile&) ;
QPair<QString,specDescriptor> readJCAMPldr(QString &first,QTextStream &in) ;
specModelItem* readJCAMPBlock(QTextStream& in) ;
void readJCAMPdata(QTextStream& in, QVector<specDataPoint>& data, double step, double xfactor, double yfactor) ;
QVector<double> waveNumbers(QTextStream&) ;
QHash<QString,specDescriptor> fileHeader(QTextStream&) ;
QList<specModelItem*> (*fileFilter(const QString& fileName)) (QFile&);
QList<double> gaussjinv(QList<QList<double> >&,QList<double>&) ;

bool comparePoints(const QPointF&, const QPointF&) ;

/*! Accumulate items equal to begin and average them in return value.
    begin will be changed to past the last accumulated item.*/
template<class T, class forwardIterator> // TODO mehr benutzen!
inline T average(forwardIterator& begin,const forwardIterator& end,
		 bool (*equal)(const T&,const T&))
{
	const T first = *begin++ ;
	T retVal = first ;
	int count = 1 ;
	while (begin != end && equal(*begin,first))
	{
		retVal += *begin++ ;
		++ count ;
	}
	return retVal /= (double) count ;
}

template<class T, class forwardIterator> // TODO mehr benutzen!
inline T average(forwardIterator& begin,const forwardIterator& end)
{
	const T first = *begin++ ;
	T retVal = first ;
	int count = 1 ;
	while (begin != end && *begin == first)
	{
		retVal += *begin++ ;
		++ count ;
	}
	return retVal /= (double) count ;
}

template<class T, class forwardIterator, class outputIterator>
inline void averageToNew(forwardIterator begin, forwardIterator end,
		  bool (*equal)(const T&,const T&), outputIterator target)
{
	while (begin != end)
		*target++ = average(begin, end, equal) ;
} ;

template<class T, class forwardIterator, class outputIterator>
inline void averageToNew(forwardIterator begin, forwardIterator end,
		  outputIterator target)
{
	while (begin != end)
		*target++ = average(begin, end) ;
} ;

#endif
