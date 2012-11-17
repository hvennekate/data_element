#include "specremovedataaction.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "cutbyintensitydialog.h"

specRemoveDataAction::specRemoveDataAction(QObject *parent)
	: specRequiresItemAction(parent)
{
	setIcon(QIcon(":/cbi.png")) ;
	setToolTip(tr("Remove data")) ;
	setWhatsThis(tr("Remove data from the selected items.  You will be prompted to determined which data points to delete by setting up ranges along the x axis."));
	setText(tr("Remove data by range...")) ;
	setShortcut(tr("Ctrl+r"));
}

const std::type_info& specRemoveDataAction::possibleParent()
{
	return typeid(specDataView) ;
}

specUndoCommand* specRemoveDataAction::generateUndoCommand()
{
	cutByIntensityDialog *dialog = new cutByIntensityDialog(parentWidget()) ;
	dialog->assignSpectra(model->pointerList(selection)) ;
	if (dialog->exec() == QDialog::Rejected)
	{
		delete dialog ;
		return 0 ;
	}

	specMultiCommand *groupCommand = new specMultiCommand ;
	groupCommand->setParentObject(model) ;
	groupCommand->setMergeable(false) ;
	QList<specRange*> ranges = dialog->ranges() ;

	foreach(QModelIndex index, selection)
	{
		specDataItem *item = dynamic_cast<specDataItem*>(model->itemPointer(index)) ;
		if (!item) continue ;
		specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
		command->setParentObject(model) ;
		command->setItem(index,item->getDataExcept(ranges)) ;
	}

	delete dialog ;

	groupCommand->setText(tr("Delete data points")) ;
	return groupCommand ;
}
