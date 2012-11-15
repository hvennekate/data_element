#ifndef SPECDATAPOINT_H
#define SPECDATAPOINT_H
#include <QDataStream>
#include <math.h>
#include <QVector>
#include "names.h"
#include <QString>

struct specDataPoint;
struct legacyDatapoint ;

QDataStream& operator<<(QDataStream&, const specDataPoint&);
QDataStream& operator>>(QDataStream&, specDataPoint&);
QDataStream& operator>>(QDataStream&, legacyDatapoint&);

struct specDataPoint{
// TODO: make inline
    double nu, sig, mint ;
	specDataPoint() ;
    specDataPoint(const double& n, const double& s, const double& m) ;
	
// Comparison operators
    bool operator<(const specDataPoint&) const ;
	bool operator==(const specDataPoint&) const ;
	bool exactlyEqual(const specDataPoint&) const ;
	bool operator!=(const specDataPoint& toCompare) const ;
	specDataPoint& operator+=(const specDataPoint&);
	specDataPoint& operator/=(const double&) ;
	specDataPoint& operator/(const double&) ;
	
	friend QDataStream& operator<<(QDataStream&, const specDataPoint&);
	friend QDataStream& operator>>(QDataStream&, specDataPoint&);
};

struct legacyDatapoint : specDataPoint
{
    friend QDataStream& operator>>(QDataStream&, legacyDatapoint&) ;
};

#endif
