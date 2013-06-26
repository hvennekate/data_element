#include "specminvariable.h"

double specMinVariable::processPoints(QVector<QPointF> &points) const
{
	double r = INFINITY ;
	foreach (const QPointF& point, points)
		r = qMin(r,point.y()) ;
	return r ;
}
