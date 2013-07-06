#include "specspectrumcalculatoraction.h"
#include <QList>
#include "specmodelitem.h"
#include "spectrumcalculatordialog.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include <muParser.h>
#include <QMessageBox>
#include "specdataview.h"
#include <QRegExp>

inline double modulo(double a, double b)
{
	return int(a) % int(b) ;
}

QString specSpectrumCalculatorAction::parameterName(int i)
{
	return "p"+QString::number(i) ;
}

QStringList specSpectrumCalculatorAction::descriptorNames(QString& teststring)
{
	QRegExp re("\"([^\"]|\\\\\")*\"") ;
	QStringList descriptorNames ;
	int i = 0 ;
	int index = 0 ;
	while((index = re.indexIn(teststring)) != -1)
	{
		descriptorNames << re.cap().mid(1,re.cap().length()-2) ;
		teststring.remove(index, re.matchedLength()) ;
		teststring.insert(index, parameterName(i++)) ;
	}
	return descriptorNames ;
}

specSpectrumCalculatorAction::specSpectrumCalculatorAction(QObject *parent) :
	specRequiresDataItemAction(parent)
{
	setIcon(QIcon::fromTheme("accessories-calculator")) ;
	setToolTip(tr("Data calculator")) ;
	setText(tr("Data Calculator")) ;
	setWhatsThis(tr("Perform mathematical operations on x- and y-data."));
}


const std::type_info& specSpectrumCalculatorAction::possibleParent()
{
	return typeid(specDataView) ;
}

specUndoCommand* specSpectrumCalculatorAction::generateUndoCommand()
{
	// prepare items
	QList<specModelItem*> items, folders ;
	expandSelectedFolders(items, folders);
	if (items.isEmpty()) return 0 ; // TODO include this in requirements

	// get formulae
	spectrumCalculatorDialog dialog ;
	if (!dialog.exec()) return 0 ;

	// prepare multi command
	specMultiCommand *parentCommand = new specMultiCommand ;
	parentCommand->setParentObject(model) ;
	parentCommand->setMergeable(false) ;

	// prepare the parsers
	mu::Parser xParser, yParser ;
	QString xFormula(dialog.xFormula()), yFormula(dialog.yFormula()) ;
	QStringList xDescriptors(descriptorNames(xFormula)), yDescriptors(descriptorNames(yFormula)) ;
	try
	{
		xParser.DefineOprt("%", modulo, 6);
		yParser.DefineOprt("%", modulo, 6);
		xParser.SetExpr(xFormula.toStdString()) ;
		yParser.SetExpr(yFormula.toStdString()) ;
	}
	catch(...)
	{
		QMessageBox::critical(0, tr("Failed evaluation"), tr("Failed parsing the given formulae.")) ;
		return 0;
	}

	// generate new data
	int failCount = 0 ;
	foreach (specModelItem* item, items)
	{
		size_t count = item->dataSize() ;
		double *oldX = new double[count],
				*oldY = new double[count],
				*newX = new double[count],
				*newY = new double[count];

		for (size_t i = 0 ; i < count ; ++i)
		{
			QPointF point = item->sample(i) ;
			oldX[i] = point.x() ;
			oldY[i] = point.y() ;
		}

		bool evaluationOk = false ;
		QVector<specDataPoint> newData ;
		try
		{
			xParser.ClearVar();
			yParser.ClearVar();
			xParser.ClearConst();
			yParser.ClearConst();

			xParser.DefineVar("x", oldX) ;
			yParser.DefineVar("x", oldX) ;
			xParser.DefineVar("y", oldY) ;
			yParser.DefineVar("y", oldY) ;
			for(int i = 0 ; i < xDescriptors.size() ; ++i)
				xParser.DefineConst(parameterName(i).toStdString(), item->descriptorValue(xDescriptors[i])) ;
			for(int i = 0 ; i < yDescriptors.size() ; ++i)
				yParser.DefineConst(parameterName(i).toStdString(), item->descriptorValue(yDescriptors[i])) ;

			xParser.Eval(newX, count) ;
			yParser.Eval(newY, count) ;

			for (size_t i = 0 ; i < count ; ++i)
				newData << specDataPoint(newX[i], newY[i], 0.) ;
			evaluationOk = true ;
		}
		catch (...) {}

		delete[] oldX ;
		delete[] oldY ;
		delete[] newX ;
		delete[] newY ;

		failCount += !evaluationOk ;

		// prepare the command
		if (evaluationOk)
		{
			if (specDataItem* dataItem = dynamic_cast<specDataItem*>(item))
				dataItem->reverseCorrection(newData) ;
			specExchangeDataCommand *command = new specExchangeDataCommand(parentCommand) ;
			command->setParentObject(model) ;
			command->setItem(model->index(item), newData);
		}
	}

	if (failCount)
		QMessageBox::warning(0,tr("Failed to evaluate"), tr("Failed to evaluate new data for ")
					 + QString::number(failCount)
					 + tr(" items. These items remained unchanged.")) ;

	// return command
	parentCommand->setText(tr("Arithmetic operation on data. New x: ")
				   + dialog.xFormula() + ". New y: "
				   + dialog.yFormula() + "." );
	return parentCommand ;
}


