#include "specaddfolderaction.h"
#include "specdataview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

specAddFolderAction::specAddFolderAction(QObject* parent)
	: specItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("folder-new"));
	setToolTip(tr("Create folder")) ;
	setWhatsThis(tr("Creates a new folder that may then be filled with items."));
	setText(tr("Add folder"));
	setShortcut(tr("f"));
}

specUndoCommand* specAddFolderAction::generateUndoCommand()
{
	specFolderItem* newItem = new specFolderItem ;
	if(! model->insertItems(QList<specModelItem*>() << newItem, insertionIndex, insertionRow))
	{
		delete newItem ;
		return 0 ;
	}
	specAddFolderCommand* command = new specAddFolderCommand ;
	command->setParentObject(model) ;
	command->setItem(newItem) ;
	command->setText(tr("Add folder")) ;
	return command ;
}
