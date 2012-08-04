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
	specDataView *currentView = (specDataView*) parent() ;
	QModelIndexList indexes = currentView->selectionModel()->selectedIndexes() ;
	specDeleteCommand *command = new specDeleteCommand() ;
	command->setItems(indexes) ;
	currentView->selectionModel()->clearSelection();

	command->setParentObject((QWidget*)parent());

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
//		command->redo();
}
