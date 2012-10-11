#include "specdeleteaction.h"
#include "specdeletecommand.h"
#include "specgenealogy.h"
#include "specmetamodel.h"
#include "specmulticommand.h"
#include "specdeleteconnectionscommand.h"
#include <QMap>
#include "speclogmodel.h"

specDeleteAction::specDeleteAction(QObject *parent) :
    specRequiresItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-delete"));
	setToolTip(tr("Delete")) ;
	setWhatsThis(tr("Deletes selected items.")) ;
	setText(tr("Delete")) ;
}

specUndoCommand* specDeleteAction::generateUndoCommand()
{
	specMultiCommand *parentCommand = 0 ;
	if (!qobject_cast<specLogModel*>(model))
	{
		specMetaModel *metaModel = model->getMetaModel() ;
		specModel *dataModel = metaModel->getDataModel() ;

		parentCommand = new specMultiCommand ;
		parentCommand->setParentObject(model) ;

		// collect all indexes and remove connections
		QModelIndexList queue = selection ;
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
			connectionsCommand->setParentObject(metaModel) ; // TODO model statt metaModel (and let the command pick the right one)
			connectionsCommand->setItems(i.key(),i.value());
		}
		parentCommand->setText(tr("Delete")) ;
		parentCommand->setMergeable(false) ;
	}

	specDeleteCommand *command = new specDeleteCommand(parentCommand) ;
	command->setParentObject(model);
	command->setItems(selection) ;
	command->setParentObject(model); // TODO: Again???
	view->selectionModel()->clearSelection();

	if (!parentCommand)
	{
		command->setText(tr("Delete")) ;
		return command ;
	}

	return parentCommand ;
}
