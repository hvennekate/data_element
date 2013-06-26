#include "specdatapoint.h"
#include <QTextStream>

QDataStream& operator<<(QDataStream& out, const specDataPoint& point)
{ out << point.nu << point.sig << point.mint ; return out ;}

QDataStream& operator>>(QDataStream& in , specDataPoint& point)
{ in  >> point.nu >> point.sig >> point.mint ; return in ; }

specDataPoint::specDataPoint() { nu = 0; sig = 0 ; mint = 0 ; }

specDataPoint::specDataPoint(const double& n, const double& s, const double& m)
{ nu = n; sig = s ; mint = m ;}

bool specDataPoint::operator<(const specDataPoint& toCompare) const
{ return nu < toCompare.nu ;}

bool specDataPoint::operator==(const specDataPoint& toCompare) const
{
	return toCompare.nu == nu ;
}

bool specDataPoint::operator!=(const specDataPoint& toCompare) const
{ return !(toCompare == *this) ; }

specDataPoint& specDataPoint::operator+=(const specDataPoint& toAdd)
{
	nu += toAdd.nu ; sig += toAdd.sig ; mint = std::max(toAdd.mint,mint) ;
	return (*this) ;
}

specDataPoint& specDataPoint::operator/=(const double& toDiv)
{
	nu /= toDiv ; sig /= toDiv ;
	return (*this) ;
}

specDataPoint& specDataPoint::operator/(const double& toDiv)
{
	*this /= toDiv ;
	return *this ;
}

bool specDataPoint::exactlyEqual(const specDataPoint &o) const
{
	return nu == o.nu && sig == o.sig && mint == o.mint ;
}

QDataStream& operator>>(QDataStream &in, legacyDatapoint& d)
{
	double t ;
	return in >> t >> d.nu >> d.sig >> d.mint ;
}
