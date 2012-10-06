#include "specfitcurve.h"
#include "lmmin.h"
#include <algorithm>

specFitCurve::fitData::fitData(mu::Parser *p)
	: parser(p),
	  x(0),
	  samples(fixedSize) //QwtSyntheticPointData(1000), // TODO adapt to plot size
{
	if (parser)
		parser->DefineVar("x",x = new double[samples.size()]) ;
}

specFitCurve::fitData::~fitData()
{
	delete[] x ;
}

QRectF specFitCurve::fitData::boundingRect() const
{
	return d_boundingRect ;
}

void specFitCurve::fitData::setRectOfInterest(const QRectF &r)
{
	d_boundingRect = r ;
}

size_t specFitCurve::fitData::size() const
{
	return samples.size() ;
}

QPointF specFitCurve::fitData::sample(size_t i) const
{
	return samples[i] ;
}

void specFitCurve::fitData::reevaluate()
{
	double *y = new double[samples.size()] ;
	const double step = d_boundingRect.width() / (samples.size()-1) ;
	double xc = d_boundingRect.left() ;
	for (int i = 0 ; i < samples.size() ; ++i)
	{
		x[i] = xc ;
		y[i] = NAN ;
		xc += step ;
	}
	if (!parser) return ;
	try
	{
		parser->Eval(y, samples.size()) ;
	}
	catch(...)
	{}
	for (int i = 0 ; i < samples.size() ; ++i)
		samples[i] = QPointF(x[i],y[i]) ;
}

specFitCurve::specFitCurve()
	: expression(specDescriptor("",spec::editable)),
	  parser(0),
	  maxSteps(0),
	  threshold(1e-2)
{
}

QStringList specFitCurve::descriptorKeys()
{
	return QStringList() << QObject::tr("Variables") <<
				QObject::tr("Fit parameters") <<
				QObject::tr("Fit expression") <<
				QObject::tr("Max fit steps") <<
				QObject::tr("Fit threshold") ;
}

void specFitCurve::refreshPlotData()
{
	fitData* fit = dynamic_cast<fitData*>(data()) ;
	if (!fit) return ;
	//////  Set variables!
	fit->reevaluate();
}

QString specFitCurve::descriptor(const QString &key, bool full)
{
	if (QObject::tr("Variables") == key)
	{
		QStringList variableDescriptors ;
		foreach(variablePair variable, variables)
			variableDescriptors << variable.first + " = " + QString::number(variable.second) ;
		if (full)
			return variableDescriptors.join("\n") ;
		if (activeVar > 0 && activeVar < variableDescriptors.size())
			return variableDescriptors[activeVar] ;
		return QString() ;
	}
	if (QObject::tr("Fit parameters") == key)
		return fitParameters.join(", ") ;
	if (QObject::tr("Fit expression") == key)
		return expression.content(full) ;
	if (QObject::tr("Max fit steps") == key)
		return QString::number(maxSteps) ;
	if (QObject::tr("Fit threshold") == key)
		return QString::number(threshold) ;
	return QString() ;
}

double specFitCurve::coerce(double val, double min, double max)
{
	return qMax(min,qMin(max,val)) ;
}

bool specFitCurve::changeDescriptor(QString key, QString value)
{
	if (QObject::tr("Variables") == key)
	{
		variables.clear();
		foreach(QString line, value.split("\n"))
		{
			line.remove(QRegExp("\\s")) ;
			if (! line.contains("=")) continue ;
			QString var = line.section("=",0,0) ;
			double value = line.section("=",1,1).toDouble() ;
			if (acceptableVariable(var))
				variables << qMakePair(var,value) ;
		}
	}
	if (QObject::tr("Fit parameters") == key)
	{
		fitParameters = value.remove(QRegExp("\\s")).split(",") ;
		fitParameters.erase(std::remove_if(fitParameters.begin(), fitParameters.end(), inacceptableVariable), fitParameters.end()) ;
		fitParameters.removeDuplicates() ;
	}
	if (QObject::tr("Fit expression") == key)
	{
		expression.setContent(value) ;
	}
	if (QObject::tr("Max fit steps") == key)
		maxSteps = qAbs(value.toInt()) ;
	if (QObject::tr("Fit threshold") == key)
		threshold = qFabs(value.toDouble()) ;
	return true ;
}

QString specFitCurve::errors()
{
	return errorString ;
}

bool specFitCurve::setActiveLine(const QString &key, int n)
{
	if (key == QObject::tr("Fit expression"))
		return expression.setActiveLine(n) ;
	if (key == QObject::tr("Variables"))
		activeVar = n ;
	return false ;
}

int specFitCurve::activeLine(const QString& key) const
{
	if (key == QObject::tr("Fit expression"))
		return expression.activeLine() ;
	if (key == QObject::tr("Variables"))
		return activeVar ;
	return 0 ;
}

bool specFitCurve::acceptableVariable(const QString &s)
{
	QRegExp re("[A-Za-z][A-Za-z0-9]*") ;
	return re.exactMatch(s) ;
}

void specFitCurve::refit(QwtSeriesData<QPointF> *data)
{
	generateParser();
	if (!parser) return ;

	double x[data->size()], y[data->size()], parameters[fitParameters.size()] ;
	// Set the fit data up
	for (size_t i = 0 ; i < data->size() ; ++i)
	{
		x[i] = data->sample(i).x() ;
		y[i] = data->sample(i).y() ;
	}
	// Set the parser up for fitting
	foreach (const variablePair& var, variables)
	{
		if (fitParameters.contains(var.first))
		{
			int index = fitParameters.indexOf(var.first) ;
			parameters[index] = var.second ;
			parser->DefineVar(var.first.toStdString(), &(parameters[index]));
		}
		else
			parser->DefineConst(var.first.toStdString(), var.second);
	}

	//// Perform the fit
	lm_status_struct status ;
	lm_control_struct control ;
	control.ftol = threshold ;
	control.maxcall = maxSteps ;
	control.printflags = 0 ;
	double currentX ;
	lmcurve_data_struct fitParams = { x, y, parser} ;
	parser->DefineVar("x",&currentX);
	lmmin(fitParameters.size(),
	      parameters,
	      data->size(),
	      (const void*) &fitParams,
	      evaluateParser,
	      &control,
	      &status,
	      0) ;

	// get the fit parameters back out:
	for (QList<variablePair>::iterator i = variables.begin() ; i != variables.end() ; ++i)
		if (fitParameters.contains(i->first))
			i->second = parameters[fitParameters.indexOf(i->first)] ;

	// set up the new parser
	generateParser();
	setParserConstants();
	setData(new fitData(parser));
}

void evaluateParser(const double *parameters, int count, const void *data, double *fitResults, int *info)
{
	Q_UNUSED(parameters) ;
	Q_UNUSED(info) ;
	lmcurve_data_struct *fitData = (lmcurve_data_struct *) data ;
	fitData->parser->DefineVar("x", ((lmcurve_data_struct*)data)->x);
	fitData->parser->Eval(fitResults, count) ;

	for (int i = 0 ; i < count ; ++i)
		fitResults[i] = fitData->y[i] - fitResults[i] ;
}

void specFitCurve::setParserConstants()
{
	foreach (const variablePair& var, variables)
		parser->DefineConst(var.first.toStdString(), var.second) ;
}

void specFitCurve::writeToStream(QDataStream &out) const
{
	out << variables <<
	       activeVar <<
	       fitParameters <<
	       expression <<
	       maxSteps <<
	       threshold <<
	       errorString ;
}

void specFitCurve::readFromStream(QDataStream &in)
{
	in >> variables >>
	      activeVar >>
	      fitParameters >>
	      expression >>
	      maxSteps >>
	      threshold >>
	      errorString ;
	generateParser();
	setParserConstants();
}

void specFitCurve::clearParser()
{
	errorString.clear();
	delete parser ;
	parser = 0 ;
}

void specFitCurve::generateParser()
{
	clearParser();
	try // this is sort of paranoid
	{
		parser->SetExpr(expression.content(true).toStdString()) ;
	}
	catch(mu::Parser::exception_type &p)
	{
		errorString = QString("Evaluation of fit expression \"%1\" failed\nReason: %2").arg(expression.content(true)).arg(p.GetMsg().c_str()) ;
		delete parser ;
		parser = 0 ;
	}
	setData(new fitData(parser));
}

specFitCurve::~specFitCurve()
{
	clearParser();
}
