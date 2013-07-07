#include "specnormalizeaction.h"
#include "ui_specnormalizeactiondialog.h"
#include <QDoubleValidator>
#include <QDialog>
#include "specmulticommand.h"
#include "specplotmovecommand.h"
#include "specdataview.h"
#include "specdataitem.h"

specNormalizeAction::specNormalizeAction(QObject *parent) :
	specRequiresDataItemAction(parent),
	uiDialog(new QDialog),
	ui(new Ui::specNormalizeActionDialog)

{
	setIcon(QIcon(":/normalize.png")) ;
	setToolTip(tr("Normalize spectrum")) ;
	setText(tr("Normalize...")) ;
	setWhatsThis(tr("Shift x and scale y for minimum or maximum to desired position/value."));
	ui->setupUi(uiDialog) ;
	ui->xValue->setValidator(new QDoubleValidator) ;
	ui->yValue->setValidator(new QDoubleValidator) ;
}

specNormalizeAction::~specNormalizeAction()
{
	delete ui ;
	delete uiDialog ;
}

const std::type_info& specNormalizeAction::possibleParent()
{
	return typeid(specDataView) ;
}

specUndoCommand* specNormalizeAction::generateUndoCommand()
{
	// Run dialog
	if (uiDialog->exec() == QDialog::Rejected) return 0 ;

	// Prepare parent command
	specMultiCommand* command = new specMultiCommand ;
	command->setParentObject(model) ;

	// Compute corrections and prepare item commands
	foreach(specModelItem* item, pointers)
	{
		if (!item->dataSize()) continue ;
		if (!dynamic_cast<specDataItem*>(item)) continue ;

		// find extremum
		size_t extremum = 0 ;
		double previousValue = item->sample(0).y() ;
		if (ui->minimum->isChecked())
		{
			for (size_t i = 0 ; i < item->dataSize() ; ++i)
				if (previousValue > item->sample(i).y())
					extremum = i, previousValue = item->sample(i).y() ;
		}
		else
		{
			for (size_t i = 0 ; i < item->dataSize() ; ++i)
				if (previousValue < item->sample(i).y())
					extremum = i, previousValue = item->sample(i).y() ;
		}

		specPlotMoveCommand* moveCommand = new specPlotMoveCommand(command) ;
		moveCommand->setItem(model->index(item)) ;
		moveCommand->setCorrections(ui->shiftXValue->isChecked() ?
							ui->xValue->text().toDouble() - item->sample(extremum).x() : 0,
						0,0,
						ui->scaleYValue->isChecked() && previousValue ? // Avoid div by 0
							ui->yValue->text().toDouble() / previousValue : 0) ;
	}

	// Set description of command
	command->setText(tr("Normalized ")
			 + QString::number(command->childCount())
			 + tr(" spectra by their ")
			 + (ui->minimum->isChecked() ? tr("minima") : tr("maxima"))
			 + tr(" to")
			 + (ui->shiftXValue->isChecked() ? tr(" x = ") + ui->xValue->text() : QString())
			 + (ui->scaleYValue->isChecked() ? tr(" y = ") + ui->yValue->text() : QString())
			 + tr("."));
	return command ;
}
