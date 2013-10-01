#ifndef SPECDATAPOINTFILTER_H
#define SPECDATAPOINTFILTER_H

#include <QDataStream>
#include "specdatapoint.h"

/*! simple data point filter */
class specDataPointFilter
{
	friend QDataStream& operator<<(QDataStream& out, const specDataPointFilter&) ;
	friend QDataStream& operator>>(QDataStream& in, specDataPointFilter&) ;
private:
	double offset, slope, factor, xshift ;
	int zeroMultiplications ; // TODO redundant
public:
	explicit specDataPointFilter(double off = 0, double sl = 0, double scale = 1, double x = 0) ;
	void applyCorrection(specDataPoint&) const ;  /*!< Korrektur anwenden. */
	void reverseCorrection(specDataPoint&) const; /*!< Korrektur rueckhaengig machen. */
	double getOffset() const { return offset ;}
	double getSlope() const { return slope ;}
	double getXShift() const { return xshift ;}
	double getFactor() const { return factor ;}
	QString description() const ;
	specDataPointFilter& operator+=(const specDataPointFilter&) ;
	bool operator==(const specDataPointFilter&) const ;
	specDataPointFilter operator-() const ;
};

#endif // SPECDATAPOINTFILTER_H
