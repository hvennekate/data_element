#include "specfiltergenerator.h"
#include "speccanvasitem.h"
#include "utility-functions.h"
#include <qwt_interval.h>

specFilterGenerator::specFilterGenerator() :
	CalcOffset(false),
	CalcSlope(false),
	CalcScale(false)
{
}

void specFilterGenerator::setReference(const QMap<double, double> &ref)
{
	reference = ref ;
	if (ref.isEmpty())
		referenceInterval = QwtInterval() ;
	else
		referenceInterval.setInterval(ref.begin().key(),
					      (ref.end() -1).key());
	refreshRanges();
}

void specFilterGenerator::calcOffset(bool doIt)
{
	CalcOffset = doIt ;
}

void specFilterGenerator::calcSlope(bool doIt)
{
	CalcSlope = doIt ;
}

void specFilterGenerator::calcScale(bool doIt)
{
	CalcScale = doIt ;
}

bool specFilterGenerator::isContainedInRanges(const double &x) const
{
	// Iterate over all ranges, if is contained:  return true
	if (!ranges.empty())
	{
		foreach(const QwtInterval& range, ranges)
			if (range.contains(x)) return true ;
		return false ;
	}

	if (referenceInterval.isValid())
	{
		return referenceInterval.contains(x) ;
	}

	return true ;
}

void specFilterGenerator::correctionSpectrum(QVector<QPointF> &spectrum)
{
	if (reference.empty() && ranges.empty()) return ;
	QVector<QPointF> newSpec ;
	foreach(const QPointF& p, spectrum)
		if (isContainedInRanges(p.x()))
			newSpec << p ;
	spectrum.swap(newSpec);
}

double specFilterGenerator::referenceValue(const double &x)
{
	if (reference.empty()) return 0 ;
	// Memoization
	if (!reference.contains(x))
	{
		if (!referenceInterval.contains(x)) return NAN ;

		QMap<double, double>::const_iterator after = reference.upperBound(x) ;
		QMap<double, double>::const_iterator before = after - 1 ;
		reference[x] = before.value()
				+ (after.value() - before.value()) /
				(after.key()   - before.key()) *
				(x - before.key()) ;
	}
	return reference[x] ;
}

void specFilterGenerator::setRanges(const QwtPlotItemList &newRanges)
{
	originalRanges.clear() ;
	foreach(const QwtPlotItem* item, newRanges)
	{
		const QwtInterval* range = dynamic_cast<const QwtInterval*>(item) ;
		if (range) originalRanges << *range ;
	}
	refreshRanges() ;
}

void specFilterGenerator::refreshRanges()
{
	ranges.clear();
	foreach(const QwtInterval& range, originalRanges)
	{
		if (referenceInterval.isValid())
			ranges << (referenceInterval & range) ;
		else
			ranges << range ;
	}
}

specDataPointFilter specFilterGenerator::generateFilter(specCanvasItem *item)
{
	if (!CalcOffset && !CalcSlope) return specDataPointFilter(NAN,NAN,NAN) ; // Nur scale macht keinen Sinn (Referenz wird skaliert) -> return invalid filter
	QVector<QPointF> spectrum = item->dataVector() ;
	correctionSpectrum(spectrum) ;
	if (spectrum.size() < CalcOffset + CalcScale + CalcSlope)
		return specDataPointFilter(NAN,NAN,NAN) ;

// Here come the macros to boost the performance
#define ACCUMULATE_X    x  += p.x() ;
#define ACCUMULATE_XX   xx += p.x()*p.x() ;
#define ACCUMULATE_XY   xy += p.x()*p.y() ;
#define ACCUMULATE_XZ   xz += p.x()*zv ;
#define ACCUMULATE_Y    y  += p.y() ;
#define ACCUMULATE_YZ   yz += p.y()*zv ;
#define ACCUMULATE_YY   yy += p.y()*p.y() ;
#define ACCUMULATE_Z    z  += zv ;
#define LOOPPOINTS(LOOPCORE) \
foreach(const QPointF& p, spectrum) { \
	double zv = (referenceValue(p.x()) - p.y()) ; \
	LOOPCORE \
		}
#define MATRIX_VECTOR_ASSIGNMENT_TWO(MATRIX_A, MATRIX_B, MATRIX_C, VECTOR_A, VECTOR_B) \
	QVector<double> vec(2) ; \
	QVector<QVector<double> > matrix(2, vec) ; \
	matrix[0][0] = MATRIX_A ; \
	matrix[1][0] = matrix[0][1] = MATRIX_B ; \
	matrix[1][1] = MATRIX_C ; \
	vec[0] = VECTOR_A ; \
	vec[1] = VECTOR_B ; \
	QVector<double> correction = gaussjinv(matrix, vec) ;

	double count = spectrum.size(),
			x = 0,
			xx = 0,
			y = 0,
			yy = 0,
			z = 0,
			xy = 0,
			xz = 0,
			yz = 0 ;
	double offset = 0, slope = 0 ;

	if(CalcOffset && !CalcSlope && !CalcScale)
	{
		LOOPPOINTS(ACCUMULATE_Z) ;
		offset = z / count ;
	}
	else if(!CalcOffset && CalcSlope && !CalcScale)
	{
		LOOPPOINTS(ACCUMULATE_XX ACCUMULATE_XZ) ;
		slope = xz / xx;
	}
	else if(CalcOffset && CalcSlope && !CalcScale)
	{
		LOOPPOINTS(ACCUMULATE_X ACCUMULATE_XX ACCUMULATE_XZ ACCUMULATE_Z) ;
		MATRIX_VECTOR_ASSIGNMENT_TWO(count, x, xx, z, xz) ;
		offset = correction[0] ;
		slope  = correction[1] ;
	}
	else if(CalcOffset && !CalcSlope && CalcScale)
	{
		LOOPPOINTS(ACCUMULATE_Y ACCUMULATE_YY ACCUMULATE_YZ ACCUMULATE_Z) ;
		MATRIX_VECTOR_ASSIGNMENT_TWO(count, y, yy, z, yz) ;
		if(correction[1] != -1.) // Nicht strikt noetig, da dann filter.valid() == false
			offset = correction[0] / (1. + correction[1]) ;
	}
	else if(!CalcOffset && CalcSlope && CalcScale)
	{
		LOOPPOINTS(ACCUMULATE_XX ACCUMULATE_XY ACCUMULATE_YY ACCUMULATE_XZ ACCUMULATE_YZ)
		MATRIX_VECTOR_ASSIGNMENT_TWO(xx, xy, yy, xz, yz) ;
		if(correction[1] != -1.) // Nicht strikt noetig, da dann filter.valid() == false
			slope = correction[0] / (1. + correction[1]) ;
	}
	else if(CalcOffset && CalcSlope && CalcScale)
	{
		LOOPPOINTS(ACCUMULATE_X  ACCUMULATE_Y  ACCUMULATE_Z
			   ACCUMULATE_XX   ACCUMULATE_XY ACCUMULATE_YY
			   ACCUMULATE_XZ   ACCUMULATE_YZ) ;
		QVector<double> vec(3) ;
		QVector<QVector<double> > matrix(3, vec) ;
		matrix[0][0] = count ;
		matrix[1][1] = xx ;
		matrix[2][2] = yy ;
		matrix[0][1] = matrix[1][0] = x ;
		matrix[0][2] = matrix[2][0] = y ;
		matrix[1][2] = matrix[2][1] = xy;
		vec[0] = z ;
		vec[1] = xz ;
		vec[2] = yz ;
		QVector<double> correction = gaussjinv(matrix, vec) ;
		if(correction[2] != -1.)
		{
			offset = correction[0] / (1. + correction[2]) ;
			slope  = correction[1] / (1. + correction[2]) ;
		}
	}

	return specDataPointFilter(offset, slope) ;
}
