#include "specyvariable.h"

QVector<double> specYVariable::values(specModelItem* item, const QVector<double>& xvals) const
{

	QVector<QPointF> points ;
	QPointF point ;

	for(size_t i = 0 ; i < item->dataSize() ; ++i)
		if(xvals.contains((point = item->sample(i)).x()))
			points << point ;
	if(points.empty()) return QVector<double> (xvals.size(), NAN) ;
	QVector<double> retVal ;
	foreach(QPointF point, points)
	retVal << point.y() ;
	return retVal ;
}

bool specYVariable::xValues(specModelItem* item, QVector<double>& xvals) const  // false if vector was empty
{
	return extractXs(item, xvals) ;
}
