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
	setText(tr("Add folder"));
	setShortcut(tr("f"));
}

specUndoCommand* specAddFolderAction::generateUndoCommand()
{
	if (! model->insertItems(QList<specModelItem*>() << new specFolderItem(), insertionIndex, insertionRow)) return 0 ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(insertionRow,0,insertionIndex)) ;

	command->setText(tr("Add folder")) ;
	command->setParentObject(model) ;
	return command ;
}
