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
	deleteCommand->setItems(foldersToFlatten) ;
	deleteCommand->redo();
	command->undo();

	command->setText(tr("Flattened ") + QString::number(foldersToFlatten.size()) + tr(" folders."));
	return command ;
}

QList<specStreamable::type> specFlattenTreeAction::requiredTypes() const
{
	return QList<specStreamable::type>() << specStreamable::folder ;
}
