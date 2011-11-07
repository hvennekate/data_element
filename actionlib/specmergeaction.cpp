#include "specmergeaction.h"
#include "specdeletecommand.h"
#include "specaddfoldercommand.h"

void specMergeAction::execute()
{
	specView *view = (specView*) parentWidget() ;
	specModel *model = view->model() ;

	// let user define similarities


	// prepare to delete the old items
	QModelIndexList indexes = currentView->selectionModel()->selectedIndexes() ; // TODO generate common code with delete action
	specDeleteCommand *removeCommand = new specDeleteCommand() ;
	removeCommand->setItems(indexes) ;
	currentView->selectionModel()->clearSelection(); // TODO select inserted items
	removeCommand->setParentWidget((QWidget*)parent());




	if (removeCommand->ok())
		library->push(removeCommand) ;
	else
		delete removeCommand ;
//		command->redo();

}
