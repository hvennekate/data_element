#include "specdeleteaction.h"
#include "specdeletecommand.h"
#include "specgenealogy.h"

specDeleteAction::specDeleteAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-delete"));
}

const std::type_info &specDeleteAction::possibleParent()
{
	return typeid(specDataView) ;
}



void specDeleteAction::execute()
{
	qDebug("starting action") ;
	specDataView *currentView = (specDataView*) parent() ;
	QModelIndexList indexes = currentView->selectionModel()->selectedIndexes() ;
	qDebug("=== getting selection") ;
	specDeleteCommand *command = new specDeleteCommand() ;
	command->setItems(indexes) ;
	qDebug("=== prepared command") ;
	currentView->selectionModel()->clearSelection();
	qDebug("=== cleared selection") ;
	if (command->ok())
		library->push(command) ;
	else
		delete command ;
//		command->redo();
	qDebug("=== pushed command onto stack") ;
}
