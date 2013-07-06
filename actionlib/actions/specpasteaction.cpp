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
	int count = model->itemPointer(insertionIndex)->children() ;
	if (! model->dropMimeData(QApplication::clipboard()->mimeData(),Qt::CopyAction,insertionRow,0,insertionIndex)) return 0 ;
	count = model->itemPointer(currentIndex)->children() - count ; // now we know, how many were inserted...
	specAddFolderCommand *command = new specAddFolderCommand ;
	QModelIndexList list ;
	for (int i = 0 ; i < count ; ++i)
		list << model->index(i+row,0,currentIndex) ;
	command->setItems(list) ;
	command->setText(tr("Paste items")) ;
	command->setParentObject(model) ;
	return command ;
}
