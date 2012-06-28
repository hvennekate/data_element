#include "specyvariable.h"

specYVariable::values(specModelItem *, const QVector<double> &) const
{

	QVector<QPointF> points ;
	QPointF point ;

	for(int i = 0 ; i < item->dataSize() ; ++i)
		if (xvals.contains((point = item->sample(i)).x()))
			points << point ;
	if (points.empty()) return QVector<double>(xvals.size(),NAN) ;
	QVector<double> retVal ;
	foreach(QPointF point, points)
		retVal << point.y() ;
	return retVal ;
}
