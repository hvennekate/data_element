#include "specintegralvariable.h"

specIntegralVariable::specIntegralVariable()
{
}

double specIntegralVariable::processPoints(QVector<QPointF> &points) const
{
	double r = 0 ;
	qSort(points.begin(),points.end(),comparePoints) ;
	QPointF previous = points.first() ; // (ought to contain at least one element at this point)
	foreach (const QPointF point, points)
	{
		QPointF diff = point - previous ;
		r += diff.x()*(diff.y()/2.+previous.y()) ;
		previous = point ;
	}
	return r ;
}
