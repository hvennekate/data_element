#include "spectreeaction.h"
#include "specmulticommand.h"
#include "specaddfoldercommand.h"
#include "specmovecommand.h"
#include "specdeletecommand.h"

specTreeAction::specTreeAction(QObject *parent) :
	specItemAction(parent)
{
	setIcon(QIcon(":/tree.png")) ;
	setToolTip(tr("Set up a directory tree")) ;
	setWhatsThis(tr("Generates a tree of directories and moves the selected items into those directories.  The columns in this dock window's list will be used from left to right to establish the level of folders within the tree."));
}


specUndoCommand *specTreeAction::generateUndoCommand()
{
	typedef QList<specModelItem*> itemPointerList ;
	typedef QPair<specFolderItem*, itemPointerList> destination ; // TODO make sub class for this
	typedef QList<destination> destinationList ;

	specFolderItem *treeRoot = new specFolderItem(0, tr("Tree")) ;
	itemPointerList items = model->pointerList(selection),
			toBeDeletedFolders ;

	// eliminate folders
	for (int i = 0 ; i < items.size() ; ++i)
	{
		if (items[i]->isFolder())
		{
			for (int j = 0 ; j < items[i]->children() ; ++j)
				items << ((specFolderItem*) (items[i]))->child(i) ;
			toBeDeletedFolders << items.takeAt(i--) ;
		}
	}

	// no doubles...:
	items = items.toSet().toList() ;

	// getting header strings
	int columnCount = model->columnCount(QModelIndex()) ;
	QStringList descriptors ;
	for (int i = 0 ; i < columnCount ; ++i)
		descriptors << model->headerData(i,Qt::Horizontal).toString() ;

	destinationList folderQueue, finalPositions ;
	folderQueue << qMakePair(treeRoot, items) ;
	foreach(QString descriptor, descriptors) // process descriptors
	{
		destinationList nextQueue ; // prepare queue for next descriptor round
		foreach(destination folder, folderQueue) // work through current queue
		{
			// create list of new categories
			QMap<QString,itemPointerList > newCategories ;
			foreach (specModelItem* item, folder.second)
				newCategories[item->descriptor(descriptor,true)] += item ;
			// examine new categories
			if (newCategories.size() < 2) // "all items the same" case -> dispose of single new category
				nextQueue << folder ;
			else
			{
				// create new categories
				destination thisParentsDirectChildren ;
				thisParentsDirectChildren.first = folder.first ;
				itemPointerList thisParentsFolderChildren ;
				foreach(const QString& newCategory, newCategories.keys())
				{
					if (newCategories[newCategory].size() == 1) // lonely item case -> no new category but retain with current parent:
					{
						// retain lonely children:
						thisParentsDirectChildren.second << newCategories[newCategory] ;
						continue ;
					}
					// otherwise:  really create new category
					specFolderItem *newFolder = new specFolderItem(0, newCategory) ;
					// and add it to the next round:
					nextQueue << qMakePair(newFolder, newCategories[newCategory]) ;
					thisParentsFolderChildren << newFolder ;
				}
				// move lonely children directly to final move list:
				finalPositions << thisParentsDirectChildren ;
				folder.first->addChildren(thisParentsFolderChildren) ;
			}
		}
		// get set for the next round
		folderQueue.swap(nextQueue) ;
	}
	// append final round's outcome to move list
	finalPositions << folderQueue ;

	// now move on to creating the undo commands:
	specMultiCommand* command = new specMultiCommand ;
	command->setText(tr("Form tree of items")) ;
	command->setParentObject(model) ;

	// add the new tree:
	int row = 0 ;
	if (!currentItem->isFolder())
	{
		row = currentItem->parent()->childNo(currentItem) ;
		currentItem = currentItem->parent() ;
	}
	while (toBeDeletedFolders.contains(currentItem)) // make sure tree won't be removed accidentally
	{
		row = currentItem->parent()->childNo(currentItem) ;
		currentItem = currentItem->parent() ;
	}
	if (! model->insertItems(QList<specModelItem*>() << treeRoot, model->index(currentItem), row))
	{
		delete treeRoot ;
		return 0 ;
	}
	specAddFolderCommand *insertTree = new specAddFolderCommand(command) ;
	insertTree->setItems(QModelIndexList() << model->index(treeRoot));
	insertTree->setParentObject(model) ;

	// move items into the new tree:
	foreach(destination d, finalPositions)
	{
		if (d.second.isEmpty()) continue ;
		specMoveCommand *moveItems = new specMoveCommand(command) ;
		moveItems->setParentObject(model) ;
		QModelIndexList indexes = model->indexList(d.second) ;
		moveItems->setItems(indexes, model->index(d.first), d.first->children());
	}

	// delete superfluous folders:  TODO: connections???
	specDeleteCommand *deleteOldFolders = new specDeleteCommand(command) ;
	deleteOldFolders->setParentObject(model) ;
	QModelIndexList indexes = model->indexList(toBeDeletedFolders) ;
	deleteOldFolders->setItems(indexes) ;

	return command ;
}
