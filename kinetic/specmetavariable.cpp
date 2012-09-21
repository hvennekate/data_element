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
#include <QDebug>

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
	specMetaVariable *product = 0;
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
		range = "[" + range + "]" ;
	}

	// setup the interval
	if (!xlower.isEmpty() || !xupper.isEmpty())
	{
		product->setMinValue( xlower.isEmpty() ? -INFINITY : xlower.toDouble()) ;
		product->setMaxValue( xupper.isEmpty() ?  INFINITY : xupper.toDouble()) ;
		product->setBorderFlags(QwtInterval::IncludeBorders);
	}

	product->parent = par ;
	product->code = range + descString ;
	return product ;
}

specMetaVariable::specMetaVariable(specMetaParser *p)
	: //QwtInterval(-INFINITY,INFINITY,QwtInterval::IncludeBorders),
	  begin(0),
	  end(int(INFINITY)),
	  inc(1),
	  refreshingRanges(false),
	  parent(p),
	  descriptor("")
{
}

bool specMetaVariable::setIndexRange(int &start, int &finish, int &step, int max) const
{
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
		if (!isValid() || contains((point = item->sample(i)).x()))
			points << point ;
	if (points.empty()) return QVector<double>(xvals.size(),NAN) ;
	return QVector<double>(xvals.size(), processPoints(points)) ;
}

bool specMetaVariable::extractXs(specModelItem *item, QVector<double> &xvals) const
{
	item->revalidate() ;
	if (xvals.size() == 1 && std::isnan(xvals[0]))
	{
		double value ;
		xvals.clear();
		for (size_t i = 0 ; i < item->dataSize() ; ++i)
			if (contains(value = item->data()->sample(i).x()))
				xvals << value ;
		return false ;
	}

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
	if (!QwtInterval::isValid()) return ; // TODO consider just disposing of the ranges
	if (plots.size() != ranges.size())
	{
		clearRanges(); // TODO : consider replotting
		foreach(specPlot* plot, plots)
		{
			specMetaRange *range = new specMetaRange(minValue(),maxValue(),this) ;
			ranges << range ;
			range->attach((QwtPlot*) plot) ;
		}
	}
	else
	{
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

QString specMetaVariable::codeValue() const
{
	if (!QwtInterval::isValid()) return code ;
	return code + QString::number(minValue()) + ":" + QString::number(maxValue()) ;
}

specMetaRange::addressObject specMetaVariable::address(specMetaRange* r)
{
	if (!parent)
	{
		specMetaRange::addressObject ao ;
		ao.item = 0 ;
		ao.range = -1 ;
		ao.variable = -1 ;
		return ao ;
	}
	specMetaRange::addressObject a = parent->addressOf(this) ;
	a.range = ranges.indexOf(r) ;
	return a;
}

void specMetaVariable::getRangePoint(int range, int point, double &x, double &y) const
{
	if (range <0 || range >= ranges.size()) return ;
	if (point != 0 && point != 1) return ;
	x = point == 0 ? minValue() : maxValue() ;
	y = ranges[range]->sample(0).y() ; // TODO this is uggahh...
}

void specMetaVariable::setRange(int rangeNo, int pointNo, double newX, double newY)
{
	if (pointNo != 0 && pointNo != 1) return ;
	if (pointNo == 0) setMinValue(newX) ;
	if (pointNo == 1) setMaxValue(newX) ;
	if (!isValid())
		setInterval(maxValue(),minValue()) ;
	foreach (specMetaRange* range, ranges)
	{
		range->setInterval(minValue(),maxValue());
		range->refreshPlotData();
	}
	if (rangeNo < 0 || rangeNo >= ranges.size()) return ;
	ranges[rangeNo]->pointMoved(pointNo, newX, newY);
}

specMetaVariable::~specMetaVariable()
{
	clearRanges();
}
