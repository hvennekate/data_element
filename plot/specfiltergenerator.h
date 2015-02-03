#ifndef SPECFILTERGENERATOR_H
#define SPECFILTERGENERATOR_H

#include <QMap>
#include <QPointF>
#include <QVector>
#include "specdatapointfilter.h"
#include <qwt_interval.h>
#include <qwt_plot_dict.h>

class QwtPlotItem ;
class specCanvasItem ;

class specFilterGenerator
{
private:
	QMap<double, double> reference ;
	QVector<QwtInterval> ranges, originalRanges ;
	QwtInterval referenceInterval ;
	bool CalcOffset, CalcSlope, CalcScale ;
	void correctionSpectrum(QVector<QPointF>&) ;
	double referenceValue(const double& x) ;
	bool isContainedInRanges(const double& x) const ;
	void refreshRanges() ;
public:
	specFilterGenerator();
	void setReference(const QMap<double, double>&) ;
	specDataPointFilter generateFilter(specCanvasItem *) ;
	void setRanges(const QwtPlotItemList& newRanges) ;
	void calcOffset(bool doIt = true) ;
	void calcSlope(bool doIt = true) ;
	void calcScale(bool doIt = true) ;
	void setMode(spec::correctionMode cMode) ;
	bool valid() const ;
};

#endif // SPECFILTERGENERATOR_H
