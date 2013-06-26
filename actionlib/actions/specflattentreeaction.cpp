#include "specflattentreeaction.h"
#include "specmulticommand.h"
#include "specmovecommand.h"
#include "specdeletecommand.h"

specFlattenTreeAction::specFlattenTreeAction(QObject *parent) :
	specRequiresItemAction(parent)
{
	setIcon(QIcon(":/notree.png")) ;
	setToolTip(tr("Flatten directories")) ;
	setWhatsThis(tr("Flattens all selected directories (one level) and places the actual data items in their places."));
}

bool specFlattenTreeAction::requirements()
{
	if (! specRequiresItemAction::requirements()) return false ;
	// Make sure we have at least one folder selected
	foreach(specModelItem* item, model->pointerList(selection))
		if (item->isFolder())
			return true ;
	return false ;
}

specUndoCommand *specFlattenTreeAction::generateUndoCommand()
{
	model->eliminateChildren(selection) ;
	QList<specModelItem*> foldersToFlatten ;
	qSort(selection) ;
	foreach(specModelItem* item, model->pointerList(selection))
		if (item->isFolder())
			foldersToFlatten << item ;
	if (foldersToFlatten.isEmpty()) return 0 ;

	specMultiCommand* command = new specMultiCommand ;
	command->setParentObject(model) ;
	foreach(specModelItem* item, foldersToFlatten)
	{
		specFolderItem* folder = dynamic_cast<specFolderItem*>(item) ;
		if (!folder) continue ;

		specMoveCommand *moveCommand = new specMoveCommand(command) ;
		QModelIndex folderIndex = model->index(folder) ;
		QModelIndexList containedIndexes = model->indexList(folder->childrenList()) ;
		moveCommand->setItems(containedIndexes,
				      model->parent(folderIndex),
				      folderIndex.row());
		moveCommand->redo();
	}

	specDeleteCommand *deleteCommand = new specDeleteCommand(command) ;
	QModelIndexList foldersToDelete = model->indexList(foldersToFlatten) ;
	deleteCommand->setItems(foldersToDelete) ;
	deleteCommand->redo();
	command->undo();

	command->setText(tr("Flattened ") + QString::number(foldersToFlatten.size()) + tr(" folders."));
	return command ;
}


