#include "specpasteaction.h"
#include "specview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

#include <QApplication>
#include <QClipboard>

specPasteAction::specPasteAction(QObject *parent) :
	specItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-paste")) ;
	setToolTip(tr("Paste")) ;
	setWhatsThis(tr("Paste data from clipboard, if possible")) ;
	setText(tr("Paste")) ;
	setShortcut(tr("Ctrl+V"));
}

specUndoCommand* specPasteAction::generateUndoCommand()
{
	specFolderItem* parentFolder = dynamic_cast<specFolderItem*>(model->itemPointer(insertionIndex)) ;
	if (!parentFolder) return 0 ;
	QList<specModelItem*> oldChildren = parentFolder->childrenList() ;
	if (!model->dropMimeData(QApplication::clipboard()->mimeData(),Qt::CopyAction,insertionRow,0,insertionIndex)) return 0 ;
	QList<specModelItem*> insertedItems ;
	foreach(specModelItem* item, parentFolder->childrenList())
		if(!oldChildren.contains(item))
			insertedItems << item ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setParentObject(model) ;
	command->setItems(insertedItems) ;
	command->setText(tr("Paste items")) ;
	return command ;
}
