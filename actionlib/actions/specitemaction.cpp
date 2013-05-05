#include "specitemaction.h"
#include "specview.h"
#include "specmodel.h"

specItemAction::specItemAction(QObject *parent)
	: specUndoAction(parent),
	  view(),
	  model(0),
      currentItem(0),
      insertionRow(0)
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

    if (currentItem && !currentItem->isFolder())
    {
        insertionRow = currentIndex.row()+1 ;
        insertionIndex = currentIndex.parent() ;

    }
    else
    {
        insertionRow = 0 ;
        insertionIndex = currentIndex ;
    }

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

void specItemAction::expandSelectedFolders(QList<specModelItem *> &items, QList<specModelItem *> &folders)
{
	if (!model) return ;
	folders.clear();
	items = model->pointerList(selection) ;
	for (int i = 0 ; i < items.size() ; ++i)
	{
	    if (items[i]->isFolder())
	    {
		for (int j = 0 ; j < items[i]->children() ; ++j)
		    items << ((specFolderItem*) (items[i]))->child(j) ;
		folders << items.takeAt(i--) ;
	    }
	}
}
