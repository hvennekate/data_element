#include "specformulavalidator.h"
#include <muParser.h>
#include <map>
#include <QStringList>
#include <QDebug>
#include "specspectrumcalculatoraction.h"

specFormulaValidator::specFormulaValidator(const QRegExp& v, QObject* parent)
	: QValidator(parent),
	  variables(v)
{
}

int specFormulaValidator::openCloseDifference(const QString& s)
{
	return s.count("(") - s.count(")") ;
}

void specFormulaValidator::fixup(QString& s) const
{
	int num = openCloseDifference(s) ;
	if(num < 0) s.prepend(QString(-num, '(')) ;
	if(num > 0) s.append(QString(num, ')')) ;
}

double mumodulo(double a, double b)
{
	return int (a) % int (b) ;
}

QValidator::State specFormulaValidator::validate(QString& expr, int& pos) const
{
	Q_UNUSED(pos)
	QString s(expr) ;
	specSpectrumCalculatorAction::descriptorNames(s) ;

	// parantheses test
	if(openCloseDifference(s))
	{
		emit evaluationError("Parantheses not balanced.");
		return Intermediate ;
	}

	// parser setup test
	mu::Parser parser ;
	parser.DefineOprt("%", mumodulo, 6);  // TODO dies ist nicht die feine englische Art!  Subclass mu::Parser and add operator(s) in custom constructor!
	std::map<mu::string_type, mu::value_type*> usedVariables ;
	try
	{
		parser.SetExpr(s.toStdString());
		usedVariables = parser.GetUsedVar() ;
	}
	catch(mu::Parser::exception_type& e)
	{
		emit evaluationError(tr("muParser error: ")
				     + QString::fromStdString(e.GetMsg())
				     + (e.GetToken().empty() ? QString() :
					"\nToken: \""
					+ QString::fromStdString(e.GetToken())));
		return Intermediate ;
	}

	// variable names test
	for(std::map<mu::string_type, mu::value_type*>::iterator
		i = usedVariables.begin() ; i != usedVariables.end() ; ++i)
	{
		QString varName(QString::fromStdString(i->first)) ;
		if(!variables.exactMatch(varName))
		{
			emit evaluationError(tr("Illegal variable name: ") + varName) ;
			return Intermediate ;
		}
	}

	emit evaluationError(QString()) ;
	return Acceptable ;
}

