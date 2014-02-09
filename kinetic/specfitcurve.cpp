#include "specfitcurve.h"
#include "lmmin.h"
#include <algorithm>
#include <gsl/gsl_cdf.h>

specFitCurve::fitData::fitData(mu::Parser* p)
	: parser(p),
	  x(0),
	  samples(fixedSize)  //QwtSyntheticPointData(1000), // TODO adapt to plot size
{
	if(parser)
		parser->DefineVar("x", x = new double[samples.size()]) ;
}

specFitCurve::fitData::~fitData()
{
	delete[] x ;
}

QRectF specFitCurve::fitData::boundingRect() const
{
	return d_boundingRect ;
}

void specFitCurve::fitData::setRectOfInterest(const QRectF& r)
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
	double* y = new double[samples.size()] ;
	const double step = d_boundingRect.width() / (samples.size() - 1) ;
	double xc = d_boundingRect.left() ;
	for(int i = 0 ; i < samples.size() ; ++i)
	{
		x[i] = xc ;
		y[i] = NAN ;
		xc += step ;
	}
	if(parser)
	{
		try
		{
			parser->Eval(y, samples.size()) ;
		}
		catch(mu::Parser::exception_type& p)
		{
			errorString = QString("Evaluation of fit expression \"%1\" failed\nReason: %2").arg(parser->GetExpr().c_str()).arg(p.GetMsg().c_str()) ;
			for(int i = 0 ; i < samples.size() ; ++i)
				samples[i] = QPointF(NAN, NAN) ;
			delete[] y ;
			return ;
		}
	}
	for(int i = 0 ; i < samples.size() ; ++i)
		samples[i] = QPointF(x[i], y[i]) ;
	delete[] y ;
}

specFitCurve::specFitCurve()
	: expression(specDescriptor("", spec::editable)),
	  parser(0),
	  variablesMulti(false),
	  expressionMulti(false),
	  messagesMulti(false)
{
	setItemAttribute(Legend, true) ;
	setLegendAttribute(LegendNoAttribute, false) ;
	setLegendAttribute(LegendShowLine, true) ;
	setLegendAttribute(LegendShowSymbol, true) ;
}

QStringList specFitCurve::descriptorKeys()
{
	return genericDescriptorKeys() ;
}

QStringList specFitCurve::genericDescriptorKeys()
{
	return QStringList() << QObject::tr("Fit variables") <<
	       QObject::tr("Fit parameters") <<
	       QObject::tr("Fit expression") <<
	       QObject::tr("Fit messages") ;
}

void specFitCurve::refreshPlotData()
{
	fitData* fit = dynamic_cast<fitData*>(data()) ;
	if(!fit) return ;
	//////  Set variables!
	fit->reevaluate();
}

QString specFitCurve::editDescriptor(const QString& key)
{
	if(key == QObject::tr("Fit variables"))
	{
		QStringList results ;
		foreach(variablePair vpair, variables)
		results << vpair.first + " = " + QString::number(vpair.second) ;
		return results.join("\n") ;
	}
	return descriptor(key, true) ;
}

QString specFitCurve::descriptor(const QString& key, bool full)
{
	if(QObject::tr("Fit variables") == key)
	{
		QStringList variableDescriptors ;
		for(int i = 0 ; i < variables.size() ; ++i)
			variableDescriptors << variables[i].first + " = " + QString::number(variables[i].second)
					    + ((i < numericalErrors.size() && !std::isnan(numericalErrors[i])) ?
					       " +/- " + QString::number(numericalErrors[i])
					       : QString(""));
		if(full || variablesMulti)
			return variableDescriptors.join("\n") ;
		if(activeVar >= 0 && activeVar < variableDescriptors.size())
			return variableDescriptors[activeVar] ;
		return QString() ;
	}
	if(QObject::tr("Fit parameters") == key)
		return fitParameters.join(", ") ;
	if(QObject::tr("Fit expression") == key)
		return expression.content(full || expressionMulti) ;
	if(QObject::tr("Fit messages") == key)
	{
		if(!errorString.isEmpty())
		{
			if(full)
				return errorString ;
			return errorString.section("\n", 0, 0) ;
		}
		if(fitData* fit = dynamic_cast<fitData*>(data()))
		{
			if(full || messagesMulti)
				return fit->errorString ;
			return fit->errorString.section("\n", 0, 0) ;
		}
	}
	return QString() ;
}

bool specFitCurve::changeDescriptor(QString key, QString value)
{
	if(QObject::tr("Fit variables") == key)
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
			if(!ok) errorValue =  NAN ;
			if(! line.contains("=")) continue ;
			QString var = line.section("=", 0, 0) ;
			double value = line.section("=", 1, 1).toDouble() ;
			if(acceptableVariable(var))
			{
				numericalErrors << errorValue ;
				variables << qMakePair(var, value) ;
			}
		}
	}
	if(QObject::tr("Fit parameters") == key)
	{
		fitParameters = value.remove(QRegExp("\\s")).split(",") ;
		fitParameters.erase(std::remove_if(fitParameters.begin(), fitParameters.end(), inacceptableVariable), fitParameters.end()) ;
		fitParameters.removeDuplicates() ;
	}
	if(QObject::tr("Fit expression") == key)
	{
		expression.setContent(value.replace("**", "^")) ;
	}

	generateParser();
	setParserConstants();
	refreshPlotData();
	return true ;
}

void specFitCurve::attach(QwtPlot* plot)
{
	qDebug() << "Data size:" << dataSize() ;
	for(size_t i  = 0 ; i < qMin((size_t) 10, dataSize()) ; ++i)
		qDebug() << sample(i) ;
	//	if (plot)
	//		if (fitData *d = dynamic_cast<fitData*>(data()))
	//			d->setRectOfInterest(plot->);
	specCanvasItem::attach(plot) ;
}

bool specFitCurve::setActiveLine(const QString& key, int n)
{
	if(key == QObject::tr("Fit expression"))
		return expression.setActiveLine(n) ;
	if(key == QObject::tr("Fit variables"))
		activeVar = n ;
	return false ;
}

int specFitCurve::activeLine(const QString& key) const
{
	if(key == QObject::tr("Fit expression"))
		return expression.activeLine() ;
	if(key == QObject::tr("Fit variables"))
		return activeVar ;
	return 0 ;
}

bool specFitCurve::acceptableVariable(const QString& s)
{
	QRegExp re("[A-Za-z][A-Za-z0-9]*(/0?.[0-9]+)?") ;
	return re.exactMatch(s) ;
}

// Diagonale der inversen Matrix = Reziproke der Diagonalelemente der L-Matrix!
void choleskyInversion(double* matrix, double* target, const int& size)
{
	// decomposition
	for(int i = 0 ; i < size ; ++i)
	{
		double sumOfSquares = 0 ;
		for(int j = 0 ; j < i ; ++j)
		{
			double sumOfProducts = 0 ;
			for(int k = 0 ; k < j ; ++k)
				sumOfProducts += target[i * size + k] * target[j * size + k] ;
			double result = (matrix[i * size + j] - sumOfProducts) / target[j * size + j] ;
			target[i * size + j] = result ;
			sumOfSquares += result * result ;
		}
		target[i * size + i] = sqrt(matrix[i * size + i] - sumOfSquares) ;
	}
	// inversion
	for(int i = 0 ; i < size ; ++i)
	{
		double diag = target[i * size + i] ;
		target[i * size + i] = 1. / diag ;
		for(int j = 0 ; j < i ; ++j)
		{
			double sumOfProducts = 0 ;
			for(int k = j ; k < i ; ++k)
				sumOfProducts += target[i * size + k] * target[k * size + j] ;
			target[i * size + j] = -sumOfProducts / diag ;
		}
	}
	// computing inverse of original matrix
	for(int i = 0 ; i < size ; ++i)
	{
		for(int j = 0 ; j <= i ; ++j)
		{
			double d = 0 ;
			for(int k = i ; k < size ; ++k)
				d += matrix[k * size + i] * matrix[k * size + j] ;
			matrix[j * size + i] = d ;
		}
	}
}

void specFitCurve::refit(QwtSeriesData<QPointF>* data)
{
	generateParser();
	if(!parser || fitParameters.isEmpty()) return ;
	QVector<std::string> variableNames(fitParameters.size()) ;

	double x[data->size()], y[data->size()], parameters[fitParameters.size()] ;
	// Set the fit data up
	for(size_t i = 0 ; i < data->size() ; ++i)
	{
		x[i] = data->sample(i).x() ;
		y[i] = data->sample(i).y() ;
	}
	// Set the parser up for fitting
	foreach(const variablePair & var, variables)
	{
		QString name = plainVariableName(var.first) ;
		if(fitParameters.contains(name))
		{
			int index = fitParameters.indexOf(name) ;
			parameters[index] = var.second ;
			parser->DefineConst(name.toStdString(), parameters[index]);
			variableNames[index] = name.toStdString() ;
		}
		else
			parser->DefineConst(name.toStdString(), var.second);
	}

	//// Perform the fit
	lm_status_struct status ;
	lm_control_struct control = lm_control_double ;

	lmcurve_data_struct fitParams = { x, y, parser, &variableNames} ;
	double* covarianceMatrix = new double [fitParameters.size() *fitParameters.size()] ;
	double sumOfSquaredResiduals = 0 ;
	lmmin(fitParameters.size(),
	      parameters,
	      data->size(),
	      (const void*) &fitParams,
	      evaluateParser,
	      &control,
	      &status,
	      0,
	      covarianceMatrix,
	      &sumOfSquaredResiduals) ;

	// if not success -> display error message
	if (status.info > 3)
		errorString = lm_infmsg[status.info] ;
	// get the fit parameters back out:
	for(QList<variablePair>::iterator i = variables.begin() ; i != variables.end() ; ++i)
	{
		QString name = plainVariableName(i->first) ;
		if(fitParameters.contains(name))
			i->second = parameters[fitParameters.indexOf(plainVariableName(name))] ;
	}

	// compute asymptotic standard error of fit parameters:
	numericalErrors.clear();
	choleskyInversion(covarianceMatrix, covarianceMatrix, fitParameters.size()) ;
	for(int i = 0 ; i < fitParameters.size() ; ++i)
	{
		for(int j = 0 ; j < fitParameters.size() ; ++j)
			qDebug() << covarianceMatrix[i * fitParameters.size() + j] ;
		qDebug() << ";" ;
	}
	foreach(const variablePair & p, variables)
	{
		QString name = plainVariableName(p.first) ;
		double confidence = confidenceInterval(p.first) ;
		int j = fitParameters.indexOf(name) ;
		int dof = data->size() - fitParameters.size() ;
		numericalErrors << (j > -1 ? sqrt(sumOfSquaredResiduals * covarianceMatrix[j * fitParameters.size() + j] / dof) *
				    ((0 < confidence && 1 > confidence) ? gsl_cdf_tdist_Pinv(confidence, dof) : 1)
					    : NAN) ;
	}
	//		errorString = QObject::tr("Covariance matrix not positive definite.") +
	//				QObject::tr("Could not determine errors.") ;
	delete [] covarianceMatrix ;

	// set up the new parser
	generateParser();
	setParserConstants();
	setData(new fitData(parser));
}

QVector<double> specFitCurve::getFitData(QwtSeriesData<QPointF>* data)
{
	double* xvals = new double[dataSize()] ;
	for(size_t i = 0 ; i < dataSize() ; ++i)
		xvals[i] = data->sample(i).x() ;
	double* oldX = (double*)(parser->GetVar().at("x")) ;
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

void evaluateParser(const double* parameters, int count, const void* data, double* fitResults, int* info)
{
	Q_UNUSED(info) ;
	lmcurve_data_struct* fitData = (lmcurve_data_struct*) data ;
	fitData->parser->DefineVar("x", ((lmcurve_data_struct*) data)->x);

	// prepare parser variables
	int j = 0 ;
	foreach(const std::string & variableName, * (fitData->variableNames))
	fitData->parser->DefineConst(variableName, parameters[j++]);

	// Evaluate and prepare for lm algorithm
	try
	{
		fitData->parser->Eval(fitResults, count) ;
	}
	catch(...)
	{
		for(int i = 0 ; i < count ; ++i)
			fitResults[i] = NAN ;
		return ;
	}
	for(int i = 0 ; i < count ; ++i)
		fitResults[i] = fitData->y[i] - fitResults[i] ;
}

void specFitCurve::setParserConstants()
{
	if(!parser) return ;
	foreach(const variablePair & var, variables)
	parser->DefineConst(plainVariableName(var.first).toStdString(), var.second) ;
}

void specFitCurve::writeToStream(QDataStream& out) const
{
	specCanvasItem::writeToStream(out);
	out << variables <<
	    activeVar <<
	    fitParameters <<
	    expression <<
	    errorString <<
	    numericalErrors ;
	if(variablesMulti || expressionMulti || messagesMulti)
		out << qint8(variablesMulti + 2 * expressionMulti + 4 * messagesMulti) ;
}

void specFitCurve::readFromStream(QDataStream& in)
{
	specCanvasItem::readFromStream(in) ;
	in >> variables >>
	   activeVar >>
	   fitParameters >>
	   expression >>
	   errorString >>
	   numericalErrors ;
	if(in.atEnd())
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
	catch(mu::Parser::exception_type& p)
	{
		errorString = QString("Evaluation of fit expression \"%1\" failed\nReason: %2").arg(expression.content(true)).arg(p.GetMsg().c_str()) ;
		delete parser ;
		parser = 0 ;
	}
	setData(new fitData(parser));

	// Make sure all fit parameters are actually variables!
	QStringList::iterator i = fitParameters.begin() ;
	while(i != fitParameters.end())
	{
		bool isVariable = false ;
		foreach(variablePair variable, variables)
		{
			if(*i == plainVariableName(variable.first))
			{
				isVariable = true ;
				break ;
			}
		}
		if(!isVariable) i = fitParameters.erase(i) ;
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
	if(key == QObject::tr("Fit variables")) variablesMulti = f & spec::multiline ;
	if(key == QObject::tr("Fit expression")) expressionMulti = f & spec::multiline ;
	if(key == QObject::tr("Fit messages")) messagesMulti = f & spec::multiline ;
}

spec::descriptorFlags specFitCurve::descriptorProperties(const QString& key) const
{
	spec::descriptorFlags flags = spec::def ;
	if(key != QObject::tr("Fit messages")) flags |= spec::editable ;
	if(key == QObject::tr("Fit variables") && variablesMulti) flags |= spec::multiline ;
	if(key == QObject::tr("Fit expression") && expressionMulti) flags |= spec::multiline ;
	if(key == QObject::tr("Fit messages") && messagesMulti) flags |= spec::multiline ;
	return flags ;
}

double specFitCurve::confidenceInterval(const QString& v) const
{
	return v.section("/", 1, 1).toDouble() ;
}

QString specFitCurve::plainVariableName(const QString& v) const
{
	return v.section("/", 0, 0) ;
}
