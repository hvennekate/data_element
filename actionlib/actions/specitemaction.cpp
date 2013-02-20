#include "specitemaction.h"
#include "specview.h"
#include "specmodel.h"

specItemAction::specItemAction(QObject *parent)
	: specUndoAction(parent),
	  view(),
	  model(0),
	  currentItem(0)
{
}

const std::type_info& specItemAction::possibleParent()
{
	return typeid(specView) ;
}

void specItemAction::execute()
{
	if (!(view = qobject_cast<specView*>(parent()))) return ;
	if (!(model = view->model())) return ;
	currentIndex = view->currentIndex() ;
	selection = view->getSelection() ;
	currentItem = model->itemPointer(currentIndex) ;
	if (!requirements()) return ;
	specUndoCommand* command = generateUndoCommand();
	if (!command) return ;
//	if (!command->ok())
//	{
//		delete command;
//		return ;
//	}
	if (!library)
	{
		command->redo();
		delete command ;
		return ;
	}
	library->push(command) ;
}
