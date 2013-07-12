#include "specdeleteaction.h"
#include "specdeletecommand.h"
#include "specgenealogy.h"
#include "specmetamodel.h"
#include "specmulticommand.h"
#include "specdeleteconnectionscommand.h"
#include <QMap>
#include "speclogmodel.h"
#include "specmetaitem.h"
#include "specdataview.h"

specDeleteAction::specDeleteAction(QObject *parent) :
	specRequiresItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-delete"));
	setToolTip(tr("Delete")) ;
	setWhatsThis(tr("Deletes selected items.")) ;
	setText(tr("Delete")) ;
	setShortcut(Qt::Key_Delete);
}

specUndoCommand* specDeleteAction::command(specModel* model, QList<specModelItem*>& selection, specUndoCommand* parentsParent)
{
	specMultiCommand *parentCommand = 0 ;
	QString descriptionText = tr("Delete ") + selection.size() + tr(" items.") ;
	if (!qobject_cast<specLogModel*>(model))
	{
		parentCommand = new specMultiCommand(parentsParent) ;
		parentCommand->setParentObject(model) ;
		parentCommand->setText(descriptionText) ;
		parentCommand->setMergeable(false) ;

		// collect all indexes and remove connections

		selection = specModel::expandFolders(selection, true) ;

		QMap<specMetaItem*, QList<specModelItem*> > toDisconnect ;
		foreach(specModelItem* item, selection)
		{
			specMetaItem* metaItem = dynamic_cast<specMetaItem*>(item) ;
			if (metaItem)
				toDisconnect[metaItem] << metaItem->serverList() ;
			foreach(specMetaItem* metaItem, item->clientList())
				toDisconnect[metaItem] << item ;
		}

		foreach(specMetaItem* metaItem, toDisconnect.keys())
			(new specDeleteConnectionsCommand(parentCommand))->
				setItems(metaItem, toDisconnect[metaItem]) ;
	}

	specDeleteCommand *command = new specDeleteCommand(parentCommand) ;
	command->setParentObject(model);
	command->setItems(selection) ;

	if (!parentCommand)
	{
		command->setText(descriptionText) ;
		return command ;
	}

	return parentCommand ;
}

specUndoCommand* specDeleteAction::generateUndoCommand()
{
	return command(model, pointers) ;
}
