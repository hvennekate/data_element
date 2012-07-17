#include "specmetavariable.h"
#include "utility-functions.h"
#include "specdescriptorvariable.h"
#include "specminvariable.h"
#include "specmaxvariable.h"
#include "specxvariable.h"
#include "specyvariable.h"
#include "specintegralvariable.h"
#include "specmetarange.h"
#include "specmetaparser.h"
#include "canvaspicker.h"

QString specMetaVariable::extract(QString & source, const QRegExp& expression)
{
	int start = expression.indexIn(source) ;
	if (start == -1) return QString() ;
	start = expression.matchedLength() ;
	QString returnValue = source.left(start) ;
	source.remove(0,start) ;
	return returnValue ;
}

specMetaVariable* specMetaVariable::factory(QString init, specMetaParser* par)
{
	const QRegExp rangeExp("(\\[[0-9]*(:[0-9]?(:[0-9]?)?)?\\])"),
			descriptorExp("(\\\"[^\\\"]*\\\"|" "(x|y|i|u|l))"),
			number("[+\\-]?([0-9]+|[0-9]*\\.[0-9]+)([eE][+-]?[0-9]+)?");
	QString range = extract(init, rangeExp) ;
	QString descString = extract(init, descriptorExp) ;
	QString xlower = extract(init, number) ;
	extract(init, QRegExp(":")) ;
	QString xupper = extract(init, number) ;

	// setting the mode of operation (consider using virtual functions/subclassing)
	specMetaVariable *product ;
	if (descString[0] == '"')
	{
		product = new specDescriptorVariable ;
		product->descriptor = descString.mid(1, descString.size()-2) ;
	}
	if (descString[0] == 'i') product = new specIntegralVariable ;
	if (descString[0] == 'u') product = new specMaxVariable ;
	if (descString[0] == 'l') product = new specMinVariable ;
	if (descString[0] == 'x') product = new specXVariable ;
	if (descString[0] == 'y') product = new specYVariable;

	if (!range.isEmpty())
	{
		//interpret the range
		QStringList indexes = range.remove("[").remove("]").split(':') ;
		product->begin = qMax(0, indexes[0] == "" ? 0 : indexes[0].toInt()) ;
		product->end   = qMax(product->begin, indexes.size()==1 ? product->begin + 1
								:(indexes[1] == "" ? int(INFINITY) : indexes[1].toInt())) ;
		product->inc   = qBound(1, indexes.size() > 2 && indexes[2] != "" ? indexes[2].toInt() : 1, product->end - product->begin) ;
	}

	// setup the interval
	product->setMinValue( xlower.isEmpty() ? -INFINITY : xlower.toDouble()) ;
	product->setMaxValue( xupper.isEmpty() ?  INFINITY : xupper.toDouble()) ;

	product->parent = par ;
	product->code = range + descString ;
	return product ;
}

specMetaVariable::specMetaVariable()
	: QwtInterval(-INFINITY,INFINITY,QwtInterval::IncludeBorders),
	  begin(0),
	  end(int(INFINITY)),
	  inc(1),
	  descriptor("")
{}

bool specMetaVariable::setIndexRange(int &start, int &finish, int &step, int max) const
{
	qDebug() << "RANGE: " << begin << end << inc ;
	start  = qBound(0,begin,max) ;
	finish = qBound(start,end,max) ;
	step   = qBound(1, inc, finish-start) ;
	return true ; // ????
}

bool specMetaVariable::xValues(specModelItem *item, QVector<double> & xvals) const // false if vector was empty
{
	Q_UNUSED(item)
	Q_UNUSED(xvals)
	return true ;
}

QVector<double> specMetaVariable::values(specModelItem *item, const QVector<double> &xvals) const
{
	QVector<QPointF> points ;
	QPointF point ;
	for (size_t i = 0 ; i < item->dataSize() ; ++i)
		if (contains((point = item->sample(i)).x()))
			points << point ;
	if (points.empty()) return QVector<double>(xvals.size(),NAN) ;
	return QVector<double>(xvals.size(), processPoints(points)) ;
}

bool specMetaVariable::extractXs(specModelItem *item, QVector<double> &xvals) const
{
	item->revalidate() ;
	qDebug() << "DATASIZE" << item->dataSize() << "RANGE" << minValue() << maxValue();
	if (xvals.size() == 1 && isnan(xvals[0]))
	{
		double value ;
		xvals.clear();
		for (size_t i = 0 ; i < item->dataSize() ; ++i)
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
			for (size_t i = 0 ; i < item->dataSize() ; ++i)
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

void specMetaVariable::clearRanges()
{
	foreach(specRange* range, ranges)
		delete range ;
	ranges.clear();
}

void specMetaVariable::produceRanges(QSet<specPlot *> plots)
{
	qDebug() << "producing ranges" << plots ;
	if (!QwtInterval::isValid()) return ; // TODO consider just disposing of the ranges
	qDebug() << "checking range list" ;
	if (plots.size() != ranges.size())
	{
		qDebug() << "creating new ranges" ;
		clearRanges();
		foreach(specPlot* plot, plots)
		{
			specMetaRange *range = new specMetaRange(minValue(),maxValue(),this) ;
			ranges << range ;
			range->attach((QwtPlot*) plot) ;
		}
		qDebug() << "ranges produced" << ranges ;
	}
	else
	{
		qDebug() << "attaching old ranges" << ranges ;
		int i = 0 ;
		foreach(specPlot* plot, plots)
			ranges[i++]->attach((QwtPlot*) plot) ;
	}
}

void specMetaVariable::detachRanges()
{
	foreach(specRange* range, ranges)
		range->detach();
}

void specMetaVariable::rangeChanged(specMetaRange* range) // TODO just take two double s
{
	qDebug() << "rangeChanged" << parent;
	setInterval(range->minValue(),range->maxValue()) ;
	if (parent)
		parent->evaluatorIntervalChanged() ;
	foreach(specRange* range, ranges)
	{
		range->setInterval(minValue(),maxValue()) ;
		range->specRange::refreshPlotData() ;
	}
}

QString specMetaVariable::codeValue()
{
	if (!QwtInterval::isValid()) return code ;
	return code + QString::number(minValue()) + ":" + QString::number(maxValue()) ;
}
