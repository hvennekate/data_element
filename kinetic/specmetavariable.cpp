#include "specmetavariable.h"
#include "utility-functions.h"

QString specMetaVariable::extract(QString & source, const QRegExp& expression) const
{
	int start = expression.indexIn(source) ;
	if (start == -1) return QString() ;
	start = expression.matchedLength() ;
	QString returnValue = source.left(start) ;
	source.remove(0,start) ;
	return returnValue ;
}

specMetaVariable::specMetaVariable(QString init)
	: begin(0),
	  end(int(INFINITY)),
	  inc(1),
	  valid(false)
{
	const QRegExp rangeExp("(\\[[0-9]*(:[0-9]?(:[0-9]?)?)?\\])"),
			descriptorExp("(\\\"[^\\\"]*\\\"|" "(x|y|i|u|l))"),
			number("[+\\-]?([0-9]+|[0-9]*\\.[0-9]+)([eE][+-]?[0-9]+)?");
	QString range = extract(init, rangeExp) ;
	qDebug() << "AFTER RANGE EXTR" << init ;
	QString descString = extract(init, descriptorExp) ;
	qDebug() << "AFTER DESCRIPTOR EXTR" << init ;
	QString xlower = extract(init, number) ;
	qDebug() << "AFTER 1ST NUMBER EXTR" << init << xlower;
	extract(init, QRegExp(":")) ;
	QString xupper = extract(init, number) ;
	qDebug() << "AFTER 2ND NUMBER EXTR" << init ;

	qDebug() << "RANGESTRING" << range ;
	if (!range.isEmpty())
	{
		//interpret the range
		QStringList indexes = range.remove("[").remove("]").split(':') ;
		qDebug() << "RANGEPARTS" << indexes << int(INFINITY);
		qDebug() << "RANGESTRINGS" << indexes ;
		begin = qMax(0, indexes[0] == "" ? 0 : indexes[0].toInt()) ;
		end   = qMax(begin, indexes.size()==1 ? begin + 1
								:(indexes[1] == "" ? int(INFINITY) : indexes[1].toInt())) ;
		inc   = qBound(1, indexes.size() > 2 && indexes[2] != "" ? indexes[2].toInt() : 1, end-begin) ;
		qDebug() << "RANGE" << begin << end << inc ;
	}

	// setting the mode of operation (consider using virtual functions/subclassing)
	if (descString[0] == '"')
	{
		descriptor = descString.mid(1, descString.size()-2) ;
		mode = 'd' ;
	}
	else
	{
		mode = descString[0] ;
		qDebug() << "CHECKING mode" << mode << descString ;
	}

	// setup the interval
	interval.setMinValue( xlower.isEmpty() ? -INFINITY : xlower.toDouble()) ;
	interval.setMaxValue( xupper.isEmpty() ?  INFINITY : xupper.toDouble()) ;
	interval.setBorderFlags(QwtInterval::IncludeBorders) ;

	valid = true ; // ????
}

bool specMetaVariable::setRange(int &start, int &finish, int &step, int max) const
{
	qDebug() << "RANGE: " << begin << end << inc ;
	start  = qBound(0,begin,max) ;
	finish = qBound(start,end,max) ;
	step   = qBound(1, inc, finish-start) ;
	return true ; // ????
}

bool specMetaVariable::xValues(specModelItem *item, QVector<double> & xvals) const // false if vector was empty
{
	if (mode != 'x' && mode != 'y') // TODO via interval.invalid()
		return true ;
	item->refreshPlotData();
	qDebug() << "DATASIZE" << item->dataSize() << "RANGE" << interval.minValue() << interval.maxValue();
	if (xvals.size() == 1 && xvals[0] == NAN)
	{
		double value ;
		xvals.clear();
		for (int i = 0 ; i < item->dataSize() ; ++i)
			if (interval.contains(value = item->data()->sample(i).x()))
				xvals << value ;
		return false ;
	}

	qDebug() << "xvals" << xvals ;
	QVector<double> newXVals ;
	foreach(double value, xvals) // TODO:  quick compare if same size
	{
		if (interval.contains(value))
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

QVector<double> specMetaVariable::values(specModelItem* item, const QVector<double>& xvals) const
{
	// No range checking!!
	if (mode == 'd')
		return QVector<double>(xvals.size(), item->descriptor(descriptor).toDouble()) ;
	if (mode == 'x') // assuming this has been checked...
		return xvals ;

	QVector<QPointF> points ;
	QPointF point ;
	for(int i = 0 ; i < item->dataSize() ; ++i)
		if (xvals.contains((point = item->sample(i)).x()))
			points << point ;
	if (points.empty()) return QVector<double>(xvals.size(),NAN) ;

	if (mode == 'y')
	{
		QVector<double> retVal ;
		foreach(QPointF point, points)
			retVal << point.y() ;
		return retVal ;
	}
	double r = 0 ;
	if (mode == 'u')
	{
		r = -INFINITY ;
		foreach (const QPointF point, points)
			r = qMax(r,point.y()) ;
	}
	else if (mode == 'l')
	{
		r = INFINITY ;
		foreach (const QPointF point, points)
			r = qMin(r,point.y()) ;
	}
	else if (mode == 'i')
	{
		qSort(points.begin(),points.end(),comparePoints) ;
		QPointF previous = points.first() ; // (ought to contain at least one element at this point)
		foreach (const QPointF point, points)
		{
			QPointF diff = point - previous ;
			r += diff.x()*(diff.y()/2.+previous.y()) ;
			previous = point ;
		}
	}

	return QVector<double>(xvals.size(),r) ;
}
