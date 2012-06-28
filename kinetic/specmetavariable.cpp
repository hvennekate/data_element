#include "specmetavariable.h"
#include "utility-functions.h"
#include "specdescriptorvariable.h"
#include "specminvariable.h"
#include "specmaxvariable.h"
#include "specxvariable.h"
#include "specyvariable.h"
#include "specintegralvariable.h"

QString specMetaVariable::extract(QString & source, const QRegExp& expression)
{
	int start = expression.indexIn(source) ;
	if (start == -1) return QString() ;
	start = expression.matchedLength() ;
	QString returnValue = source.left(start) ;
	source.remove(0,start) ;
	return returnValue ;
}

specMetaVariable* specMetaVariable::factory(QString init)
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

	return product ;
}

specMetaVariable::specMetaVariable()
	: specRange(-INFINITY,INFINITY),
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
	return true ;
}

QVector<double> specMetaVariable::values(specModelItem *item, const QVector<double> &xvals) const
{
	QVector<QPointF> points ;
	QPointF point ;
	for (int i = 0 ; i < item->dataSize() ; ++i)
		if (contains((point = item->sample(i)).x()))
			points << point ;
	if (points.empty()) return QVector<double>(xvals.size(),NAN) ;
	return QVector<double>(xvals.size(), processPoints(points)) ;
}
