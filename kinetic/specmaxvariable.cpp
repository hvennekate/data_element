#include "specmaxvariable.h"

double specMaxVariable::processPoints(QVector<QPointF> &points) const
{
	r = -INFINITY ;
	foreach (const QPointF point, points)
		r = qMax(r,point.y()) ;
}
