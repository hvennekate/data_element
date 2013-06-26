#include "speccopyaction.h"
#include "specview.h"
#include <QApplication>
#include <QClipboard>

specCopyAction::specCopyAction(QObject *parent) :
	specRequiresItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-copy")) ;
	setToolTip(tr("Copy selected items")) ;
	setWhatsThis(tr("Copy items to clipboard.  May be pasted in this program or in any other that understands plain text.")) ;
	setText(tr("Copy")) ;
	setShortcut(tr("Ctrl+c"));
}

specUndoCommand* specCopyAction::generateUndoCommand()
{
	copyToClipboard(model,selection) ;
	return 0 ;
}

void specCopyAction::copyToClipboard(specModel *model, const QModelIndexList& selection)
{
	QApplication::clipboard()->setMimeData(model->mimeData(selection));
}
