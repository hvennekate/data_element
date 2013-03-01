#include "specfitcurve.h"
#include "lmmin.h"
#include <algorithm>

#ifndef WIN32BUILD
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#endif

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
	reevaluate() ;
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
	if (parser)
	{
		try
		{
				parser->Eval(y, samples.size()) ;
		}
		catch(mu::Parser::exception_type &p)
		{
			errorString = QString("Evaluation of fit expression \"%1\" failed\nReason: %2").arg(parser->GetExpr().c_str()).arg(p.GetMsg().c_str()) ;
			for (int i = 0 ; i < samples.size() ; ++i)
				samples[i] = QPointF(NAN,NAN) ;
			delete[] y ;
			return ;
		}
	}
	for (int i = 0 ; i < samples.size() ; ++i)
		samples[i] = QPointF(x[i],y[i]) ;
	delete[] y ;
}

specFitCurve::specFitCurve()
	: expression(specDescriptor("",spec::editable)),
      parser(0),
      variablesMulti(false),
      expressionMulti(false),
      messagesMulti(false)
{
}

QStringList specFitCurve::descriptorKeys()
{
	return QStringList() << QObject::tr("Fit variables") <<
				QObject::tr("Fit parameters") <<
				QObject::tr("Fit expression") <<
				QObject::tr("Fit messages") ;
}

void specFitCurve::refreshPlotData()
{
	fitData* fit = dynamic_cast<fitData*>(data()) ;
	if (!fit) return ;
	//////  Set variables!
	fit->reevaluate();
}

QString specFitCurve::editDescriptor(const QString &key)
{
    if (key == QObject::tr("Fit variables"))
    {
        QStringList results ;
        foreach(variablePair vpair, variables)
            results << vpair.first + " = " + QString::number(vpair.second) ;
        return results.join("\n") ;
    }
    return descriptor(key, true) ;
}

QString specFitCurve::descriptor(const QString &key, bool full)
{
	if (QObject::tr("Fit variables") == key)
	{
		QStringList variableDescriptors ;
        for (int i = 0 ; i < variables.size() ; ++i)
            variableDescriptors << variables[i].first + " = " + QString::number(variables[i].second)
                                   + ((i < numericalErrors.size() && !std::isnan(numericalErrors[i])) ?
                                          " +/- " + QString::number(numericalErrors[i])
                                        : QString(""));
        if (full || variablesMulti)
			return variableDescriptors.join("\n") ;
		if (activeVar >= 0 && activeVar < variableDescriptors.size())
			return variableDescriptors[activeVar] ;
		return QString() ;
	}
	if (QObject::tr("Fit parameters") == key)
		return fitParameters.join(", ") ;
	if (QObject::tr("Fit expression") == key)
        return expression.content(full || expressionMulti) ;
	if (QObject::tr("Fit messages") == key)
	{
		if (!errorString.isEmpty())
		{
			if (full)
				return errorString ;
			return errorString.section("\n",0,0) ;
		}
		if (fitData* fit = dynamic_cast<fitData*>(data()))
		{
            if (full || messagesMulti)
				return fit->errorString ;
			return fit->errorString.section("\n",0,0) ;
		}
	}
	return QString() ;
}

double specFitCurve::coerce(double val, double min, double max)
{
	return qMax(min,qMin(max,val)) ;
}

bool specFitCurve::changeDescriptor(QString key, QString value)
{
	if (QObject::tr("Fit variables") == key)
	{
		variables.clear();
        numericalErrors.clear();
		foreach(QString line, value.split("\n"))
		{
            line.remove(QRegExp("\\s")) ;
            double errorValue = NAN ;
            bool ok = true ;
            QStringList l = line.split("+/-") ;
            line = l.first() ;
            errorValue = (l.size() > 1 ? l[1].toDouble(&ok) : NAN) ;
            if (!ok) errorValue =  NAN ;
            if (! line.contains("=")) continue ;
			QString var = line.section("=",0,0) ;
			double value = line.section("=",1,1).toDouble() ;
			if (acceptableVariable(var))
            {
                numericalErrors << errorValue ;
                variables << qMakePair(var,value) ;
            }
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
		expression.setContent(value.replace("**","^")) ;
	}

	generateParser();
	setParserConstants();
	refreshPlotData();
	return true ;
}

void specFitCurve::attach(QwtPlot *plot)
{
//	if (plot)
//		if (fitData *d = dynamic_cast<fitData*>(data()))
//			d->setRectOfInterest(plot->);
	specCanvasItem::attach(plot) ;
}

bool specFitCurve::setActiveLine(const QString &key, int n)
{
	if (key == QObject::tr("Fit expression"))
		return expression.setActiveLine(n) ;
	if (key == QObject::tr("Fit variables"))
		activeVar = n ;
	return false ;
}

int specFitCurve::activeLine(const QString& key) const
{
	if (key == QObject::tr("Fit expression"))
		return expression.activeLine() ;
	if (key == QObject::tr("Fit variables"))
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
    if (fitParameters.isEmpty()) return ;
    QVector<std::string> variableNames(fitParameters.size()) ;

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
			parser->DefineConst(var.first.toStdString(), parameters[index]);
            variableNames[index] = var.first.toStdString() ;
		}
		else
			parser->DefineConst(var.first.toStdString(), var.second);
	}

	//// Perform the fit
	lm_status_struct status ;
	lm_control_struct control = lm_control_double ;

	lmcurve_data_struct fitParams = { x, y, parser, &variableNames} ;
#ifndef WIN32BUILD
    gsl_matrix *covarianceMatrix = gsl_matrix_alloc(fitParameters.size(),fitParameters.size()) ;
    double sumOfSquaredResiduals = 0 ;
#endif
	lmmin(fitParameters.size(),
	      parameters,
	      data->size(),
	      (const void*) &fitParams,
	      evaluateParser,
	      &control,
	      &status,
          0,
#ifdef WIN32BUILD
          0, 0
#else
          covarianceMatrix->data,
          &sumOfSquaredResiduals
#endif
          ) ;
	// get the fit parameters back out:
	for (QList<variablePair>::iterator i = variables.begin() ; i != variables.end() ; ++i)
		if (fitParameters.contains(i->first))
			i->second = parameters[fitParameters.indexOf(i->first)] ;

#ifndef WIN32BUILD
    // compute asymptotic standard error of fit parameters:
    gsl_set_error_handler_off() ;
    numericalErrors.clear();
    if (GSL_EDOM != gsl_linalg_cholesky_decomp(covarianceMatrix))
    {
        gsl_linalg_cholesky_invert(covarianceMatrix) ;
        for(QList<variablePair>::Iterator i = variables.begin() ; i != variables.end() ; ++i)
        {
            int j = fitParameters.indexOf(i->first) ;
            numericalErrors << (j > -1 ?
                                  sqrt(gsl_matrix_get(covarianceMatrix,j,j)*sumOfSquaredResiduals / (data->size()-fitParameters.size()))
                                  : NAN) ;
        }
    }
    else
    {
        errorString = QObject::tr("Covariance matrix not positive definite.") +
                  QObject::tr("Could not determine errors.") ;
    }

    gsl_matrix_free(covarianceMatrix) ;
#endif

    // set up the new parser
    generateParser();
    setParserConstants();
    setData(new fitData(parser));
}

QVector<double> specFitCurve::getFitData(QwtSeriesData<QPointF> *data)
{
    double *xvals = new double[dataSize()] ;
    for (size_t i = 0 ; i < dataSize() ; ++i)
        xvals[i] = data->sample(i).x() ;
    double *oldX = (double*) (parser->GetVar().at("x")) ;
    parser->DefineVar("x", xvals);

    QVector<double> yValues(dataSize(), NAN) ;

    try
    {
        parser->Eval(yValues.data(), dataSize()) ;
    }
    catch(...)
    {
    }

    parser->DefineVar("x", oldX) ;
    delete[] xvals ;
    return yValues ;
}

void evaluateParser(const double *parameters, int count, const void *data, double *fitResults, int *info)
{
	Q_UNUSED(info) ;
	lmcurve_data_struct *fitData = (lmcurve_data_struct *) data ;
	fitData->parser->DefineVar("x", ((lmcurve_data_struct*)data)->x);

	// prepare parser variables
	int j = 0 ;
	foreach(const std::string& variableName, *(fitData->variableNames))
		fitData->parser->DefineConst(variableName, parameters[j++]);

	// Evaluate and prepare for lm algorithm
	try
	{
		fitData->parser->Eval(fitResults, count) ;
	}
	catch(...)
	{
		for (int i = 0 ; i < count ; ++i)
			fitResults[i] = NAN ;
		return ;
	}
	for (int i = 0 ; i < count ; ++i)
		fitResults[i] = fitData->y[i] - fitResults[i] ;
}

void specFitCurve::setParserConstants()
{
    if (!parser) return ;
	foreach (const variablePair& var, variables)
		parser->DefineConst(var.first.toStdString(), var.second) ;
}

void specFitCurve::writeToStream(QDataStream &out) const
{
	specCanvasItem::writeToStream(out);
	out << variables <<
	       activeVar <<
	       fitParameters <<
	       expression <<
           errorString <<
           numericalErrors ;
    if (variablesMulti || expressionMulti || messagesMulti)
        out << qint8(variablesMulti + 2*expressionMulti + 4*messagesMulti) ;
}

void specFitCurve::readFromStream(QDataStream &in)
{
    specCanvasItem::readFromStream(in) ;
    in >> variables >>
          activeVar >>
          fitParameters >>
          expression >>
          errorString >>
          numericalErrors ;
    if (in.atEnd())
        expressionMulti = messagesMulti = variablesMulti = false ;
    else
    {
        qint8 a ;
        in >> a ;
        messagesMulti = a & qint8(4) ;
        expressionMulti = a & qint8(2) ;
        variablesMulti = a & qint8(1) ;
    }
    generateParser();
    setParserConstants();
//	setData(new fitData(parser)) ;
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
	parser = new mu::Parser ;
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

    // Make sure all fit parameters are actually variables!
    QStringList::iterator i = fitParameters.begin() ;
    while (i != fitParameters.end())
    {
        bool isVariable = false ;
        foreach(variablePair variable, variables)
        {
            if (*i == variable.first)
            {
                isVariable = true ;
                break ;
            }
        }
        if (!isVariable) i = fitParameters.erase(i) ;
        else ++i ;
    }
}

specFitCurve::~specFitCurve()
{
	clearParser();
}

void specFitCurve::setDescriptorProperties(QString key, spec::descriptorFlags f)
{
    // TODO implement
    if (key == QObject::tr("Fit variables")) variablesMulti = f & spec::multiline ;
    if (key == QObject::tr("Fit expression")) expressionMulti = f & spec::multiline ;
    if (key == QObject::tr("Fit messages")) messagesMulti = f & spec::multiline ;
}

spec::descriptorFlags specFitCurve::descriptorProperties(const QString &key) const
{
    spec::descriptorFlags flags = spec::def ;
    if (key != QObject::tr("Fit messages")) flags |= spec::editable ;
    if (key == QObject::tr("Fit variables") && variablesMulti) flags |= spec::multiline ;
    if (key == QObject::tr("Fit expression") && expressionMulti) flags |= spec::multiline ;
    if (key == QObject::tr("Fit messages") && messagesMulti) flags |= spec::multiline ;
    return flags ;
}
