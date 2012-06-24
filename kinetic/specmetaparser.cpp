#include "specmetaparser.h"
#include <QStringList>
#include <QRegExp>
#include <qwt/qwt_series_data.h>
#include <iostream>

specMetaParser::specMetaParser(const QString &expressionList, const QString& xExpression, const QString& yExpression)
{
	setAssignments(expressionList, xExpression, yExpression) ;
}

void specMetaParser::clear()
{
	foreach(specMetaVariable* var, evaluators)
		delete var ;
	evaluators.clear();
	symbols.remove_all() ;
	errors.clear();

}

void specMetaParser::setAssignments(const QString &expressionList, const QString& xExpression, const QString& yExpression)
{
	qDebug() << "SETTING PARSER VARIABLES" << expressionList.toAscii() << xExpression.toAscii() << yExpression.toAscii() << xExpression.size() ;
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
	//   - x,y,i,u,l -> x, y, integral, upper, lower mit optionaler range/punkt
	//  Bei jeder Auswertung fuer ein item werden die Ergebnisarrays an das
	//  master-array fuer das jeweilige Symbol angefuegt.  Padding, wenn ein
	//  symbol-array mehr als ein neues Element bekommt.  Wenn mehrere mehr als ein Element:
	//  x-Werte pruefen, matchen.
	//
	//  Dann x, y auswerten.
	qDebug() << "REGEXP" << "(\\[[0-9]*(:[0-9]*(:[0-9]*)?)?\\])?"
				"(\"[^\"]*\"|"
				"((x|y|i|u|l)"
				"([+\\-]?([0-9]+|[0-9]*.[0-9]+)([eE][0-9]+)?)?:"
				"([+\\-]?([0-9]+|[0-9]*.[0-9]+)([eE][0-9]+)?)?)" ;
	const QRegExp acceptable(//"(\\[[0-9]*(:[0-9]*(:[0-9]*)?)?\\])?"
				 "(\"[^\"]*\"|"
				 "((x|y|i|u|l)"
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
		if (expression.count("=") != 1)
		{
			errors << QString("May contain exactly one \"=\":  %1").arg(expression) ;
			continue ;
		}
		QString symbol = expression.section(assignment,0,0) ;
		QString value  = expression.section(assignment,1,1) ;
		if (!name.exactMatch(symbol))
		{
			errors << QString("Not an acceptable variable name:  %1").arg(symbol) ;
			valid = false ;
			continue ;
		}
		if (!acceptable.exactMatch(value))
		{
			errors << QString("Not an acceptable value:  %1").arg(value) ;
			valid = false ;
			continue ;
		}
		symbols.append(GiNaC::symbol(symbol.toStdString())) ;
		evaluators << new specMetaVariable(value) ;
	}
	qDebug() << "xExpression" << xExpression  << (xExpression == "x") << "yExpression" << yExpression ;
	x = prepare(xExpression) ;
	y = prepare(yExpression) ;
//	x = prepare("x") ;
//	y = prepare("y") ;
	xExp = xExpression ;
	yExp = yExpression ;
	std::cerr << "x-Expression: " << x << "\ny-Expression: " << y << "\n" ;
}

QwtSeriesData<QPointF>* specMetaParser::evaluate(const QVector<specModelItem*>& items)
{
	QVector<QPointF> data ;
	QVector<QVector<int> > indexes(evaluators.size(),QVector<int>(3));
	for (int i = 0 ; i < evaluators.size() ; ++i)
		evaluators[i]->setRange(indexes[i][0], indexes[i][1], indexes[i][2], items.size()) ; // TODO do it over
	qDebug() << "RANGES:"<< indexes ;
	int j = 0 ;
	while (true)
	{
		qDebug() << "starting loop" ;
		QVector<specModelItem*> currentItems ;
		QVector<double> xValues(1,NAN) ; // start value in case no arrays are among our variables
		qDebug() << "XVALUES parent" << xValues ;

		// preparing a list of current items and overlapping xValues
		if (indexes.isEmpty()) break ;
		for(int i = 0 ; i < indexes.size() ; ++i)
		{
			// check for exhaustion
			int index = indexes[i][0]+indexes[i][2]*j ;
			qDebug() << "current index" << index << items.size() ;
			if (!(index < indexes[i][1])) break ;
			currentItems << items[index] ;
			evaluators[i]->xValues(items[index],xValues) ;
		}
		if (currentItems.size() != indexes.size()) break ; // termination for while loop

		qDebug() << "filling substitution list" << evaluators.size() << xValues.size() ;
		QVector<QVector<double> > substitutions(xValues.size(),QVector<double>(evaluators.size())) ;
		for(int i = 0 ; i < evaluators.size() ; ++i)
		{
			QVector<double> values = evaluators[i]->values(currentItems[i],xValues) ;
			qDebug() << "VALUES:" << values << i << evaluators.size() ;
			for (int k = 0 ; k < values.size() ; ++k)
				substitutions[k][i] = values[k] ; // Matrix kippen...
		}

		qDebug() << "evaluating" ;
		for (int i = 0 ; i < substitutions.size() ; ++i)
		{
			if (containsNan(substitutions[i]))
			{
				data << QPointF(NAN, NAN) ;
				continue ;
			}
			GiNaC::exmap substitution ;
			qDebug() << "substitution:" << substitutions[i] ;
			for (int k = 0 ; k < substitutions[i].size() ; ++k)
				substitution[symbols[k]] = substitutions[i][k] ;

			GiNaC::ex xEx = GiNaC::evalf(x.subs(substitution, GiNaC::subs_options::no_pattern)),
					yEx = GiNaC::evalf(y.subs(substitution, GiNaC::subs_options::no_pattern)) ;
			if (GiNaC::is_a<GiNaC::numeric>(xEx) && GiNaC::is_a<GiNaC::numeric>(yEx))
			{
				qDebug() << "Success" ;
				data << QPointF(GiNaC::ex_to<GiNaC::numeric>(xEx).to_double(),
					GiNaC::ex_to<GiNaC::numeric>(yEx).to_double()) ;
			}
		}
		++j ;
	}
	qDebug() << "data size before returning:" << data.size() << data ;
	return new QwtPointSeriesData(data) ;
}

QString specMetaParser::warnings() const
{
	return errors.join("\n") ;
}

bool specMetaParser::ok() const
{
	return valid ;
}

GiNaC::ex specMetaParser::prepare(const QString &val)
{
	GiNaC::ex retVal ;
	try
	{
		GiNaC::symtab tab ;
		foreach(GiNaC::ex expr, symbols)
		{
			GiNaC::symbol symbol = GiNaC::ex_to<GiNaC::symbol>(expr) ;
			tab[symbol.get_name()] = symbol ;
		}
		GiNaC::parser parse(tab) ;
		parse.strict = true ;
		retVal = parse(val.toStdString()) ;
//		retVal = GiNaC::ex(val.toStdString(),symbols) ;
	}
	catch(std::exception &p)
	{
		errors << QString("Evaluation of GiNaC-Expression \"%1\" failed\nReason: %2").arg(val).arg(p.what()) ;
		valid = false ;
	}
	return  retVal ;
}

bool specMetaParser::containsNan(const QVector<double> &vector)
{
	foreach(double zahl, vector)
		if (isnan(zahl))
			return true ;
	return false ;
}
