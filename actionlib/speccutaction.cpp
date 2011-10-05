#include "speccutaction.h"
#include <QApplication>
#include <QClipboard>

specCutAction::specCutAction(QObject *parent) :
    specDeleteAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-cut")) ;
}

void specCutAction::execute()
{
	specView *currentView = (specView*) parent() ;
	QApplication::clipboard()->setMimeData(
	currentView->model()->mimeData(currentView->selectionModel()->selectedIndexes()) );
	specDeleteAction::execute() ;
}
