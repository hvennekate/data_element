#include "specremovedataaction.h"
#include "specmulticommand.h"
#include "specremovedatacommand.h"
#include "cutbyintensitydialog.h"

specRemoveDataAction::specRemoveDataAction(QObject *parent)
	: specUndoAction(parent)
{
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
		specRemoveDataCommand *command = new specRemoveDataCommand(groupCommand) ;
		QVector<int> dataPoints ;
		specModelItem *item = view->model()->itemPointer(index) ;
		for (int i = 0 ; i < item->dataSize() ; ++i)
		{
			for (int j = 0 ; j < ranges.size() ; ++j)
			{
				if (ranges[j]->contains(item->data()->sample(i).x()))
				{
					dataPoints << i ;
					break ;
				}
			}
		}
		command->setParentWidget(view) ;
		command->setItem(index,dataPoints) ;
		if (!command->ok())
			delete command ; // TODO check if this is ok without telling parent
	}

	delete dialog ;

	qDebug() << "Pushing group command with" << groupCommand->childCount() << "children." ;

	library->push(groupCommand);
}
