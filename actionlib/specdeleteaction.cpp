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
	specView *currentView = (specView*) parent() ;
	QModelIndexList indexes = currentView->selectionModel()->selectedIndexes() ;
	specDeleteCommand *command = new specDeleteCommand() ;
	command->setItems(indexes) ;
	currentView->selectionModel()->clearSelection();

	command->setParentObject(currentView->model());

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
//		command->redo();
}
