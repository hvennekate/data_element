#include "specmetaparser.h"
#include <QStringList>
#include <QRegExp>
#include <qwt/qwt_series_data.h>

specMetaParser::specMetaParser(const QString &expressionList, const QString& xExpression, const QString& yExpression)
{
	setAssignments(expressionList, xExpression, yExpression) ;
}

void specMetaParser::clear()
{
	foreach(specMetaVariable* var, evaluators)
		delete var ;
	evaluators.clear();
}

void specMetaParser::setAssignments(const QString &expressionList, const QString& xExpression, const QString& yExpression)
{
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
	errors.clear();
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
	x = prepare(xExpression) ;
	y = prepare(yExpression) ;
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
		QVector<specModelItem*> currentItems ;
		QVector<double> xValues(1,NAN) ; // start value in case no arrays are among our variables
		qDebug() << "XVALUES parent" << xValues ;
		bool filled = false;
		for(int i = 0 ; i < indexes.size() ; ++i)
		{
			// check for exhaustion
			int index = indexes[i][0]+indexes[i][2]*j ;
			currentItems << (index > indexes[i][1] ? 0 : items[index]) ;
			if (!(evaluators[i]->xValues(items[index],xValues)))
				if (filled) // Don't fill it twice! (conflicting selections)
					break ;

			filled = true ;
		}
		if (currentItems.contains(0)) break ; // termination for while loop

		QVector<GiNaC::lst> substitutions(xValues.size()) ;
		for(int i = 0 ; i < evaluators.size() ; ++i)
		{
			QVector<double> values = evaluators[i]->values(currentItems[i],xValues) ;
			for (int k = 0 ; k < values.size() ; ++k)
				substitutions[k].append(symbols[k] == values[k]) ;
		}

		for (int i = 0 ; i < substitutions.size() ; ++i)
		{
			GiNaC::ex xEx = GiNaC::evalf(x.subs(substitutions[i])),
			   yEx = GiNaC::evalf(y.subs(substitutions[i])) ;
			if (! GiNaC::is_a<GiNaC::numeric>(x) || !GiNaC::is_a<GiNaC::numeric>(y))
				break ;
			data << QPointF(GiNaC::ex_to<GiNaC::numeric>(xEx).to_double(),
					GiNaC::ex_to<GiNaC::numeric>(yEx).to_double()) ;
		}

		++j ;
	}
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
		retVal = GiNaC::ex(val.toStdString(),symbols) ;
	}
	catch(std::exception &p)
	{
		errors << QString("Evaluation of GiNaC-Expression \"%1\" failed\nReason: %2").arg(val).arg(p.what()) ;
		valid = false ;
	}
	return  retVal ;
}
