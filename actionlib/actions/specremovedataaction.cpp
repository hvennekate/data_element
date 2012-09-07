#include "specremovedataaction.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "cutbyintensitydialog.h"

specRemoveDataAction::specRemoveDataAction(QObject *parent)
	: specUndoAction(parent)
{
	setIcon(QIcon(":/cbi.png")) ;
	setToolTip(tr("Remove data")) ;
	setWhatsThis(tr("Remove data from the selected items.  You will be prompted to determined which data points to delete by setting up ranges along the x axis."));
}

void specRemoveDataAction::execute()
{
	specDataView *view = dynamic_cast<specDataView*>(parentWidget()) ;
	cutByIntensityDialog *dialog = new cutByIntensityDialog(parentWidget()) ;
	QModelIndexList indexes = view->getSelection() ;
	dialog->assignSpectra(view->model()->pointerList(indexes)) ;
	if (dialog->exec() == QDialog::Rejected)
	{
		delete dialog ;
		return ;
	}

	specMultiCommand *groupCommand = new specMultiCommand ;
	groupCommand->setParentObject(view->model()) ;
	groupCommand->setMergeable(false) ;
	QList<specRange*> ranges = dialog->ranges() ;

	foreach(QModelIndex index, indexes)
	{
		specDataItem *item = dynamic_cast<specDataItem*>(view->model()->itemPointer(index)) ;
		if (!item) continue ;
		specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
		command->setParentObject(view->model()) ;
		command->setItem(index,item->getDataExcept(ranges)) ;
	}

	delete dialog ;

	groupCommand->setText(tr("Delete data points")) ;
	library->push(groupCommand);
}
