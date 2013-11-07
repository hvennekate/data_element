#include "specdatapointfilter.h"

QDataStream& operator << (QDataStream& out, const specDataPointFilter& f)
{
	return out << f.offset
	       << f.slope
	       << f.factor
	       << f.xshift
	       << f.zeroMultiplications ;
}

QDataStream& operator >> (QDataStream& in, specDataPointFilter& f)
{
	return in >> f.offset
	       >> f.slope
	       >> f.factor
	       >> f.xshift
	       << f.zeroMultiplications ;
}

specDataPointFilter& specDataPointFilter::operator += (const specDataPointFilter& other)
{
	xshift += other.xshift ;
	if(0 == other.factor)
		zeroMultiplications ++ ;
	else if(INFINITY == other.factor)
		zeroMultiplications = 0 ;
	else if(!isnan(other.factor))
	{
		slope  *= other.factor ;
		factor *= other.factor ;
		offset *= other.factor ;

		offset += other.offset ;
		slope  += other.slope  ;
	}
	return *this;
}

specDataPointFilter specDataPointFilter::operator -() const
{
	if(factor == 0) return specDataPointFilter(offset,
				       slope,
				       INFINITY,
				       -xshift) ;
	if(factor == INFINITY) return specDataPointFilter(offset,
					      slope,
					      0,
					      -xshift) ;
	return specDataPointFilter(-factor / offset,
				   -factor / slope,
				   1 / factor,
				   -xshift) ;
}

void specDataPointFilter::applyCorrection(specDataPoint& point) const
{
	point.nu += xshift ;
	point.sig = zeroMultiplications ? 0 : point.sig * factor + offset + slope * point.nu ;
}

void specDataPointFilter::reverseCorrection(specDataPoint& point) const
{
	point.sig = (point.sig - offset - slope * point.nu) / factor ;
	point.nu = point.nu - xshift ;
}

specDataPointFilter::specDataPointFilter(double off, double sl, double scale, double x)
	: offset(off),
	  slope(sl),
	  factor(scale),
	  xshift(x),
	  zeroMultiplications(0)
{}

QString specDataPointFilter::description() const
{
	return QObject::tr("Offset: ") + QString::number(offset)
	       + QObject::tr(", slope: ") + QString::number(slope)
	       + QObject::tr(", scaling: ") + QString::number(factor)
	       + QObject::tr(", x shift:") + QString::number(xshift) ;
}

bool specDataPointFilter::operator == (const specDataPointFilter& other) const
{
	return (factor == other.factor &&
		xshift == other.xshift &&
		offset == other.offset &&
		slope  == other.slope) ;
}
