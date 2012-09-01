#include "specdeleteaction.h"
#include "specdeletecommand.h"
#include "specgenealogy.h"
#include "specmetamodel.h"
#include "specmulticommand.h"
#include "specdeleteconnectionscommand.h"
#include <QMap>

specDeleteAction::specDeleteAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-delete"));
	setToolTip(tr("Delete")) ;
	setWhatsThis(tr("Deletes selected items.")) ;
}

const std::type_info &specDeleteAction::possibleParent()
{
	return typeid(specDataView) ;
}

void specDeleteAction::execute()
{
	specView *currentView = (specView*) parent() ;
	QModelIndexList indexes = currentView->selectionModel()->selectedIndexes() ;
	// TODO change this if there are no connections
	specMultiCommand *parentCommand = new specMultiCommand ;
	parentCommand->setParentObject(currentView->model()) ;

	specMetaModel *metaModel = currentView->model()->getMetaModel() ;
	specModel *dataModel = metaModel->getDataModel() ;

	// collect all indexes and remove connections
	QModelIndexList queue = indexes ;
	QMap<specModelItem*, QModelIndex> allItems ;

	while (!queue.isEmpty())
	{
		QModelIndex item = queue.takeFirst() ;
		specModel *model = (specModel*) (item.model()) ;
		specModelItem *itemPointer = model->itemPointer(item) ;
		allItems[itemPointer] = item ;
		if (itemPointer->children())
			for (int i = 0 ; i < itemPointer->children() ; ++i)
				queue << model->index(((specFolderItem*) itemPointer)->child(i)) ;
	}

	QMap<QModelIndex, QModelIndexList> toDisconnect ;
	for (QMap<specModelItem*, QModelIndex>::iterator i = allItems.begin() ; i != allItems.end() ; ++i)
	{
		if (i.value().model() == metaModel)
		{
			foreach (specModelItem* server, i.key()->serverList())
			{
				QModelIndex serverIndex = dataModel->index(server) ;
				if (!serverIndex.isValid())
					serverIndex = metaModel->index(server) ;
				toDisconnect[i.value()] << serverIndex ;
			}
		}
		foreach (specMetaItem *client, i.key()->clientList())
		{
			QModelIndex clientIndex = metaModel->index((specModelItem*) client) ;
			if (!toDisconnect[clientIndex].contains(i.value()))
				toDisconnect[clientIndex] << i.value() ;
		}
	}

	for(QMap<QModelIndex,QModelIndexList>::iterator i = toDisconnect.begin() ; i != toDisconnect.end() ; ++i)
	{
		specDeleteConnectionsCommand *connectionsCommand = new specDeleteConnectionsCommand(parentCommand) ;
		connectionsCommand->setParentObject(metaModel) ;
		connectionsCommand->setItems(i.key(),i.value());
	}

	specDeleteCommand *command = new specDeleteCommand(parentCommand) ;
	command->setParentObject(currentView->model());
	command->setItems(indexes) ;
	command->setParentObject(currentView->model());

	currentView->selectionModel()->clearSelection();
	if (command->ok())
		library->push(parentCommand) ;
	else
		delete parentCommand ;
}
