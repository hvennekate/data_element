#include "specdatapoint.h"
#include <QTextStream>

QDataStream& operator<<(QDataStream& out, const specDataPoint& point)
{ out << point.t << point.nu << point.sig << point.mint ; return out ;}
QDataStream& operator>>(QDataStream& in , specDataPoint& point)
{ in  >> point.t >> point.nu >> point.sig >> point.mint ; return in ; }

specDataPoint::specDataPoint() { nu = 0; sig = 0 ; mint = 0 ; t = 0; }

specDataPoint::specDataPoint(const double& time, const double& n, const double& s, const double& m)
{ t = time ; nu = n; sig = s ; mint = m ;}

specDataPoint::specDataPoint(const QVector<double>& content)
{
	t    = content.size() > 0 ? content[0] : 0 ;
	nu   = content.size() > 1 ? content[1] : 0 ;
	sig  = content.size() > 2 ? content[2] : 0 ;
	mint = content.size() > 3 ? content[3] : 0 ;
}

specDataPoint specDataPoint::operator=(const QVector<double>& vec)
{ *this = specDataPoint(vec) ; return (*this) ;}

double specDataPoint::operator[](int index) const
{ return index == 0 ? t : index == 1 ? nu : index == 2 ? sig : index == 3 ? mint : NAN ; }

bool specDataPoint::operator<(const specDataPoint& toCompare) const
{ return ( toCompare.nu == nu ? t < toCompare.t : nu < toCompare.nu);}
// { return ( toCompare.t == t ? nu < toCompare.nu : t < toCompare.t);}


bool specDataPoint::operator==(const specDataPoint& toCompare) const
{ return toCompare.t == t && toCompare.nu == nu ; }
// TODO Introduce time tolerance

bool specDataPoint::operator>(const specDataPoint& toCompare) const
{ return ( toCompare.t == t ? nu > toCompare.nu : t > toCompare.t );}

bool specDataPoint::operator!=(const specDataPoint& toCompare) const
{ return !(toCompare == *this) ; }

specDataPoint& specDataPoint::operator+=(const specDataPoint& toAdd)
{
	t += toAdd.t ; nu += toAdd.nu ; sig += toAdd.sig ; mint = std::max(toAdd.mint,mint) ;
	return (*this) ;
}

specDataPoint& specDataPoint::operator/=(const double& toDiv)
{
	t /= toDiv ; nu /= toDiv ; sig /= toDiv ;
	return (*this) ;
}

specDataPoint& specDataPoint::operator/(const double& toDiv)
{
	specDataPoint toReturn = *this ;
	toReturn /= toDiv ;
	return toReturn ;
}
