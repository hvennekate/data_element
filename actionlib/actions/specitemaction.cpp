#include "specitemaction.h"
#include "specview.h"
#include "specmodel.h"
#include "specmulticommand.h"

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
	QList<specStreamable::type> types = requiredTypes() ;
	pointers.clear();

	// Recursively eliminate folders (TODO think this through!)
	if (!types.contains(specStreamable::folder))
		model->expandFolders(selection);

	if (!types.isEmpty())
	{
		foreach(specModelItem* item, model->pointerList(selection))
			if (types.contains(item->typeId()))
				pointers << item ;
	}
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

	if (dynamic_cast<specMultiCommand*>(commad) && !command->childCount())
	{
		delete command ;
		return ;
	}

	if (!library)
	{
		command->redo();
		delete command ;
		return ;
	}
	library->push(command) ;
}

void specItemAction::expandSelectedFolders(QList<specModelItem *> &items, QList<specModelItem *> &folders) ////////// TODO make available to all item actions (subclass)!!!!!
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

QList<specStreamable::type> specItemAction::requiredTypes() const
{
	return QList<specStreamable::type>() ;
}
