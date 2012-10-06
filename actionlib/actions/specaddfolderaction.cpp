#include "specaddfolderaction.h"
#include "specdataview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

specAddFolderAction::specAddFolderAction(QObject *parent)
	: specItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("folder-new"));
	setToolTip(tr("Create folder")) ;
	setWhatsThis(tr("Creates a new folder that may then be filled with items."));
}

specUndoCommand* specAddFolderAction::generateUndoCommand()
{
	int row = 0 ;
	if (currentItem && !currentItem->isFolder())
	{
		row = currentIndex.row()+1 ;
		currentIndex = currentIndex.parent() ;

	}
	if (! model->insertItems(QList<specModelItem*>() << new specFolderItem(), currentIndex, row)) return 0 ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(row,0,currentIndex)) ;

	command->setText(tr("Add folder")) ;
	command->setParentObject(model) ;
	return command ;
}
