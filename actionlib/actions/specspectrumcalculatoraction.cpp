#include "specspectrumcalculatoraction.h"
#include <QList>
#include "specmodelitem.h"
#include "spectrumcalculatordialog.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include <muParser.h>
#include <QMessageBox>
#include "specdataview.h"

specSpectrumCalculatorAction::specSpectrumCalculatorAction(QObject *parent) :
	specRequiresItemAction(parent)
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
	if (items.isEmpty()) return 0 ;

	// get formulae
	spectrumCalculatorDialog dialog ;
	if (!dialog.exec()) return 0 ;

	// prepare multi command
	specMultiCommand *parentCommand = new specMultiCommand ;
	parentCommand->setParentObject(model) ;
	parentCommand->setMergeable(false) ;

	// prepare the parsers
	mu::Parser xParser, yParser ;
	try
	{
		xParser.SetExpr(dialog.xFormula().toStdString()) ;
		yParser.SetExpr(dialog.yFormula().toStdString()) ;
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

			xParser.DefineVar("x", oldX) ;
			yParser.DefineVar("x", oldX) ;
			xParser.DefineVar("y", oldY) ;
			yParser.DefineVar("y", oldY) ;

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
