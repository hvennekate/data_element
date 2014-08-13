#include "specmetaparser.h"
#include <QStringList>
#include <QRegExp>
#include <muParser.h>
#include <qwt_series_data.h>
#include <iostream>
#include "specmetaitem.h"

specMetaParser::specMetaParser(const QString& expressionList, const QString& xExpression, const QString& yExpression, specMetaItem* par)
	: parent(par),
	  x(0),
	  y(0),
	  changingRange(false)
{
	setAssignments(expressionList, xExpression, yExpression) ;
}

void specMetaParser::clear()
{
	foreach(specMetaVariable * var, evaluators)
	delete var ;
	evaluators.clear();
	symbols.clear();
	valueVector.clear();
	errors.clear();
	delete x ;
	delete y ;
	x = 0 ;
	y = 0 ;
}

void specMetaParser::setAssignments(const QString& expressionList, const QString& xExpression, const QString& yExpression)
{
	if(changingRange) return ;
	valid = true ;
	const QRegExp name("[a-zA-Z][a-zA-Z0-9]*") ;
	clear() ;
	// Syntax:
	//  [start:stop:incr]"string"
	// oder:
	//  [start:stop:incr]x|y|i|u|l<zahl>:<zahl>  beide <zahl> optional!
	// Strategie:
	//  Jedes symbol ist ein Array aus den beteiligten items und wird ausgewertet nach
	//   - index ([i], [i:j], [:j], [:],[::inc] etc) -> nur diese items heranziehen
	//   - "string" -> diesen Deskriptor verwenden
	//   - x,y,i,u,l,p,P -> x, y, integral, upper, lower mit optionaler range/punkt, lower/upper x value
	//  Bei jeder Auswertung fuer ein item werden die Ergebnisarrays an das
	//  master-array fuer das jeweilige Symbol angefuegt.  Padding, wenn ein
	//  symbol-array mehr als ein neues Element bekommt.  Wenn mehrere mehr als ein Element:
	//  x-Werte pruefen, matchen.
	//
	//  Dann x, y auswerten.
	const QRegExp acceptable("(\\[[0-9]*(:[0-9]*(:[0-9]*)?)?\\])?"
				 "(\"[^\"]*\"|"
				 "((x|y|i|u|l|p|P)"
				 "("
				 "(([+\\-]?([0-9]+|[0-9]*.[0-9]+)([eE][+-]?[0-9]+)?)?"
				 "(:[+\\-]?([0-9]+|[0-9]*.[0-9]+)([eE][+-]?[0-9]+)?)?)|"
				 "(([+\\-]?([0-9]+|[0-9]*.[0-9]+)([eE][+-]?[0-9]+)?:)?"
				 "([+\\-]?([0-9]+|[0-9]*.[0-9]+)([eE][+-]?[0-9]+)?)?))"
				 "))") ;
	const QRegExp assignment(" *= *") ;
	QStringList expressions = expressionList.split("\n") ;
	foreach(QString expression, expressions)
	{
		if(expression.count("=") != 1)
		{
			errors << QString("May contain exactly one \"=\":  %1").arg(expression) ;
			continue ;
		}
		QString symbol = expression.section(assignment, 0, 0) ;
		QString value  = expression.section(assignment, 1, 1) ;
		if(!name.exactMatch(symbol))
		{
			errors << QString("Not an acceptable variable name:  ") + symbol ;
			valid = false ;
			continue ;
		}
		if(!acceptable.exactMatch(value))
		{
			errors << QString("Not an acceptable value:  ") + value ;
			valid = false ;
			continue ;
		}
		symbols << symbol ;
		evaluators << specMetaVariable::factory(value, this) ;
	}
	evaluators.removeAll(0) ;
	valueVector.resize(symbols.size());
	x = prepare(xExpression) ;
	y = prepare(yExpression) ;

	xExp = xExpression ;
	yExp = yExpression ;
}

void specMetaParser::itemsToQuery(const QVector<specModelItem*>& items,
				  QVector<QVector<specModelItem*> >& itemsPerPoint)
{
	if(evaluators.isEmpty() || items.isEmpty()) return ;
	typedef specMetaVariable::indexLimit trackingIndex ;
	typedef QVector<trackingIndex> trackingIndexes ;
	trackingIndexes indexes ;
	foreach(specMetaVariable * variable, evaluators)
	indexes << variable->indexRange(items.size()) ;  // TODO functionality envy.
	// get the items per point
	while(true)
	{
		QVector<specModelItem*> itemsForThisPoint ;
		for(trackingIndexes::iterator i = indexes.begin() ; i != indexes.end() ; ++i)
		{
			if(i->begin < i->end)
				itemsForThisPoint << items[i->begin] ;
			i->begin += i->increment ;
		}
		if(itemsForThisPoint.size() < evaluators.size()) break ;  // some index(es) reached its(their) limit(s)
		itemsPerPoint << itemsForThisPoint ;
	}
}

void specMetaParser::getVariableValues(const QVector<specModelItem*>& itemsToUse,
				       QVector<QVector<double> >& variableValues)
{
	if(itemsToUse.size() != evaluators.size()) return ;

	// Build xValues
	QVector<double>  xValues(1, NAN) ;
	for(int i = 0 ; i < evaluators.size() ; ++i)
		evaluators[i]->xValues(itemsToUse[i], xValues) ;

	// Build results
	// get values
	QVector<QVector<double> > values ;
	for(int i = 0 ; i < evaluators.size() ; ++i)
		values << evaluators[i]->values(itemsToUse[i], xValues) ;

	// flip matrix
	for(int i = 0 ; i < xValues.size() ; ++i)
	{
		QVector<double> row ;
		for(int j = 0 ; j < evaluators.size() ; ++j)  // TODO additional condition:  enough values
			row << values[j][i] ;
		variableValues << row ;
	}
}

void specMetaParser::getPoints(const QVector<QVector<double> >& variableValues,
			       QVector<QPointF>& result)
{
	if(variableValues.isEmpty()) return ;
	if(variableValues.first().isEmpty()) return ;
	for(int i = 0 ; i < variableValues.size() ; ++i)
	{
		if(containsNan(variableValues[i]))
		{
			result << QPointF(NAN, NAN) ;
			continue ;
		}
		for(int j = 0 ; j < valueVector.size() ; ++j)
			valueVector[j] = variableValues[i][j] ;
		if (!x || !y) return ;
		try
		{
			result << QPointF(x->Eval(), y->Eval()) ;
		}
		catch(mu::Parser::exception_type& p)
		{
			errors << QObject::tr("Evaluation of muParser-Expression failed\nReason: ") + p.GetMsg().c_str() ;
			valid = false ;
		}
	}
}

QwtSeriesData<QPointF>* specMetaParser::evaluate(const QVector<specModelItem*>& items)
{
	QVector<QVector<specModelItem*> > itemsPerPoint ;
	QVector<QVector<double> > variableValues ;
	QVector<QPointF> data ;
	itemsToQuery(items, itemsPerPoint) ;
	for(QVector<QVector<specModelItem*> >::Iterator i = itemsPerPoint.begin() ; i != itemsPerPoint.end() ; ++i)
		getVariableValues(*i, variableValues) ;
	getPoints(variableValues, data) ;
	return new QwtPointSeriesData(data) ;
}


/*
QwtSeriesData<QPointF>* specMetaParser::evaluate(const QVector<specModelItem*>& items)
{
	QVector<QPointF> data ;
	QVector<QVector<int> > indexes(evaluators.size(),QVector<int>(3));
	for (int i = 0 ; i < evaluators.size() ; ++i)
	evaluators[i]->indexRange(indexes[i][0], indexes[i][1], indexes[i][2], items.size()) ; // TODO do it over
	int j = 0 ;
	while (true)
	{
		QVector<specModelItem*> currentItems ;
		QVector<double> xValues(1,NAN) ; // start value in case no arrays are among our variables

		// TODO very fragile code...
		// preparing a list of current items and overlapping xValues
		if (indexes.isEmpty()) break ;
		for(int i = 0 ; i < indexes.size() ; ++i)
		{
			// check for exhaustion
			int index = indexes[i][0]+indexes[i][2]*j ;
			if (!(index < indexes[i][1])) break ;
			currentItems << items[index] ;
	    evaluators[i]->xValues(items[index],xValues) ;
		}
		if (currentItems.size() != indexes.size()) break ; // termination for while loop

		QVector<QVector<double> > substitutions(xValues.size(),QVector<double>(evaluators.size())) ;
		for(int i = 0 ; i < evaluators.size() ; ++i)
		{
	    QVector<double> values = evaluators[i]->values(currentItems[i],xValues) ;
			for (int k = 0 ; k < values.size() ; ++k)
				substitutions[k][i] = values[k] ; // Matrix kippen...
		}

		for (int i = 0 ; i < substitutions.size() ; ++i)
		{
			if (containsNan(substitutions[i]))
			{
				data << QPointF(NAN, NAN) ;
				continue ;
			}

			// By all means avoid reallocation of the valueVector!!!
			for (int k = 0 ; k < valueVector.size() ; ++k)
				valueVector[k] = substitutions[i][k] ;

			try
			{
				data << QPointF(x.Eval(),y.Eval()) ;
			}
			catch(mu::Parser::exception_type &p)
			{
		errors << tr("Evaluation of muParser-Expression failed\nReason: %1").arg(p.GetMsg().c_str()) ;
				valid = false ;
			}
		}
		++j ;
	}
	return new QwtPointSeriesData(data) ;
}
*/

QString specMetaParser::warnings() const
{
	return errors.join("\n") ;
}

bool specMetaParser::ok() const
{
	return valid ;
}

mu::Parser* specMetaParser::prepare(const QString& val)
{
	mu::Parser *retVal = new mu::Parser ;
	try
	{
		int i = 0 ;
		foreach(QString symbol, symbols)
			retVal->DefineVar(symbol.toStdString(), & (valueVector[i++]));
		retVal->SetExpr(val.toStdString()) ;
	}
	catch(mu::Parser::exception_type& p)
	{
		errors << QString("Evaluation of muParser-Expression \"%1\" failed\nReason: %2").arg(val).arg(p.GetMsg().c_str()) ;
		valid = false ;
	}
	return  retVal ;
}

bool specMetaParser::containsNan(const QVector<double>& vector)
{
	foreach(double zahl, vector)
	if(std::isnan(zahl))
		return true ;
	return false ;
}

bool specMetaParser::containsNan(const QVector<QVector<double> >& matrix, int column)
{
	for(int i = 0 ; i < matrix.size() ; ++i)
		if(matrix[i].size() <= column || std::isnan(matrix[i][column]))
			return true ;
	return false ;
}

void specMetaParser::attachRanges(QSet<specPlot*> plots, QColor color)
{
	foreach(specMetaVariable * evaluator, evaluators)
		evaluator->produceRanges(plots, color) ;
}

void specMetaParser::detachRanges()
{
	foreach(specMetaVariable * evaluator, evaluators)
		evaluator->detachRanges();
}

specMetaRange::addressObject specMetaParser::addressOf(specMetaVariable* v) const
{
	specMetaRange::addressObject a ;
	a.item = parent ;
	a.range = -1 ;
	a.variable = evaluators.indexOf(v) ;
	return a ;
}

void specMetaParser::getRangePoint(int variable, int range, int point, double& x, double& y) const
{
	if(variable < 0 || variable >= evaluators.size()) return ;
	evaluators[variable]->getRangePoint(range, point, x, y) ;
}

void specMetaParser::setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY)
{
	if(variableNo < 0 || variableNo >= evaluators.size()) return ;
	evaluators[variableNo]->setRange(rangeNo, pointNo, newX, newY) ;
	changingRange = true ;
	if(parent)
	{
		QStringList descriptor;
		QStringList::const_iterator j = symbols.begin() ;
		for(QList<specMetaVariable*>::const_iterator i = evaluators.begin() ;
			i < evaluators.end() && j != symbols.end() ; ++i, ++j)
			descriptor += *j + " = " + (*i)->codeValue() ; // TODO this is a pain...
		parent->changeDescriptor("variables", descriptor.join("\n")) ;
		parent->setActiveLine("variables", variableNo) ;
	}
	changingRange = false ;

}
