#include "specremovedataaction.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "cutbyintensitydialog.h"

specRemoveDataAction::specRemoveDataAction(QObject *parent)
	: specUndoAction(parent)
{
	setIcon(QIcon(":/cbi.png")) ;
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
	groupCommand->setParentWidget(view) ;
	groupCommand->setMergeable(false) ;
	QList<specRange*> ranges = dialog->ranges() ;

	foreach(QModelIndex index, indexes)
	{
		specDataItem *item = dynamic_cast<specDataItem*>(view->model()->itemPointer(index)) ;
		if (!item) continue ;
		specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
		command->setParentWidget(view) ;
		command->setItem(index,item->getDataExcept(ranges)) ;
	}

	delete dialog ;

	qDebug() << "Pushing group command with" << groupCommand->childCount() << "children." ;

	library->push(groupCommand);
}
