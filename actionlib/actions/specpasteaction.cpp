#include "specpasteaction.h"
#include "specview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"
#include <QMessageBox>
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
	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(checkClipboard())) ;
	checkClipboard();
}

void specPasteAction::checkClipboard()
{
	setEnabled(model->mimeAcceptable(QApplication::clipboard()->mimeData())) ;
}

bool specPasteAction::specificRequirements()
{
	return isEnabled() ;
}

specUndoCommand* specPasteAction::generateUndoCommand()
{
	specFolderItem* parentFolder = dynamic_cast<specFolderItem*>(model->itemPointer(insertionIndex)) ;
	if (!parentFolder) return 0 ;
	QList<specModelItem*> oldChildren = parentFolder->childrenList() ;
	if (!model->dropMimeData(QApplication::clipboard()->mimeData(),Qt::CopyAction,insertionRow,0,insertionIndex))
		QMessageBox::critical(0,tr("Paste error"), tr("Could not paste correctly."));
	return 0 ;
}
