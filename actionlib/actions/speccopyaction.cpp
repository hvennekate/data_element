#include "speccopyaction.h"
#include "specview.h"
#include <QApplication>
#include <QClipboard>

specCopyAction::specCopyAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-copy")) ;
	setToolTip(tr("Copy selected items")) ;
	setWhatsThis(tr("Copy items to clipboard.  May be pasted in this program or in any other that understands plain text.")) ;
}

const std::type_info &specCopyAction::possibleParent()
{
	return typeid(specView) ;
}

void specCopyAction::execute()
{
	specView *currentView = (specView*) parent() ;
	QApplication::clipboard()->setMimeData(
	currentView->model()->mimeData(currentView->selectionModel()->selectedIndexes()) );
}
