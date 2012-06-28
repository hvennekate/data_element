#include "specyvariable.h"

QVector<double> specYVariable::values(specModelItem *item, const QVector<double> &xvals) const
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

bool specYVariable::xValues(specModelItem *item, QVector<double> & xvals) const // false if vector was empty
{
	item->revalidate() ;
	qDebug() << "DATASIZE" << item->dataSize() << "RANGE" << minValue() << maxValue();
	if (xvals.size() == 1 && isnan(xvals[0]))
	{
		double value ;
		xvals.clear();
		for (int i = 0 ; i < item->dataSize() ; ++i)
			if (contains(value = item->data()->sample(i).x()))
				xvals << value ;
		return false ;
	}

	qDebug() << "xvals" << xvals << xvals.size() << (isnan(xvals[0]));
	QVector<double> newXVals ;
	foreach(double value, xvals) // TODO:  quick compare if same size
	{
		if (contains(value))
		{
			for (int i = 0 ; i < item->dataSize() ; ++i)
			{
				if(item->data()->sample(i).x() == value)
				{
					newXVals << value ;
					continue ;
				}
			}
		}
	}
	xvals = newXVals ; // TODO: swap
	return true ;
}
