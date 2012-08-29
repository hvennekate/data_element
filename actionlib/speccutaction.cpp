#include "speccutaction.h"
#include <QApplication>
#include <QClipboard>

specCutAction::specCutAction(QObject *parent) :
    specDeleteAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-cut")) ;
	setToolTip(tr("Cut selected items")) ;
	setWhatsThis(tr("Cut items to clipboard.  Removes orginal items from this file.  May be pasted in this program or in any other that understands plain text.")) ;
}

void specCutAction::execute()
{
	specView *currentView = (specView*) parent() ;
	QApplication::clipboard()->setMimeData(
	currentView->model()->mimeData(currentView->selectionModel()->selectedIndexes()) );
	specDeleteAction::execute() ;
}
