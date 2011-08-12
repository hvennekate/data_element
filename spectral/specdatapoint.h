#ifndef SPECDATAPOINT_H
#define SPECDATAPOINT_H
#include <QDataStream>
#include <math.h>
#include <QVector>
#include "names.h"
#include <QString>

struct specDataPoint;

QDataStream& operator<<(QDataStream&, const specDataPoint&);
QDataStream& operator>>(QDataStream&, specDataPoint&);

struct specDataPoint{
// TODO: make inline
	double t, nu, sig, mint ;
	specDataPoint() ;
	specDataPoint(const double& time, const double& n, const double& s, const double& m) ;
	specDataPoint(const QVector<double>& content) ;
	specDataPoint operator=(const QVector<double>& vec) ;
	double operator[](int index) const ;
	
// Comparison operators
	bool operator<(const specDataPoint&) const ;
	bool operator==(const specDataPoint&) const ;
	bool operator>(const specDataPoint&) const ;
	bool operator!=(const specDataPoint& toCompare) const ;
	specDataPoint& operator+=(const specDataPoint&);
	specDataPoint& operator/=(const double&) ;
	specDataPoint& operator/(const double&) ;
	
	friend QDataStream& operator<<(QDataStream&, const specDataPoint&);
	friend QDataStream& operator>>(QDataStream&, specDataPoint&);
};

#endif
