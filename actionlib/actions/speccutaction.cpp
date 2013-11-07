#include "speccutaction.h"
#include <QApplication>
#include <QClipboard>
#include "speccopyaction.h"

specCutAction::specCutAction(QObject* parent) :
	specDeleteAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-cut")) ;
	setToolTip(tr("Cut selected items")) ;
	setWhatsThis(tr("Cut items to clipboard.  Removes orginal items from this file.  May be pasted in this program or in any other that understands plain text.")) ;
	setText(tr("Cut")) ;
	setShortcut(QKeySequence("Ctrl+x"));
}

specUndoCommand* specCutAction::generateUndoCommand()
{
	specCopyAction::copyToClipboard(model, selection) ;
	return specDeleteAction::generateUndoCommand() ;
}
