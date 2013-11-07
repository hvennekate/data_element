#include "spectreeaction.h"
#include "specmulticommand.h"
#include "specaddfoldercommand.h"
#include "specmovecommand.h"
#include "specdeletecommand.h"
#include "specworkerthread.h"

class treeActionThread : public specWorkerThread
{
private:
	typedef QList<specModelItem*> itemPointerList ;

	specUndoCommand* Command ;
	specFolderItem* treeRoot ;
	specModel* model ;
	specModelItem* currentItem ;
	itemPointerList items, toBeDeletedFolders ;

	bool cleanUp()
	{
		if(!toTerminate) return false ;
		delete Command ;
		Command = 0 ;
		delete treeRoot ;
		treeRoot = 0 ;
		return true ;
	}
public:
	void run()
	{
		emit progressValue(0) ;
		typedef QPair<specFolderItem*, itemPointerList> destination ; // TODO make sub class for this
		typedef QList<destination> destinationList ;

		treeRoot = new specFolderItem(0, tr("Tree")) ;

		// no doubles...:
		items = items.toSet().toList() ;

		// getting header strings
		int columnCount = model->columnCount(QModelIndex()) ;
		QStringList descriptors ;
		for(int i = 0 ; i < columnCount ; ++i)
			descriptors << model->headerData(i, Qt::Horizontal).toString() ;

		destinationList folderQueue, finalPositions ;
		folderQueue << qMakePair(treeRoot, items) ;
		emit progressValue(5);
		foreach(QString descriptor, descriptors)  // process descriptors
		{
			if(cleanUp()) return ;
			destinationList nextQueue ; // prepare queue for next descriptor round
			foreach(destination folder, folderQueue)  // work through current queue
			{
				// create list of new categories
				QMap<QString, itemPointerList > newCategories ;
				foreach(specModelItem * item, folder.second)
				newCategories[item->descriptor(descriptor, true)] += item ;
				// examine new categories
				if(newCategories.size() < 2)  // "all items the same" case -> dispose of single new category
					nextQueue << folder ;
				else
				{
					// create new categories
					destination thisParentsDirectChildren ;
					thisParentsDirectChildren.first = folder.first ;
					itemPointerList thisParentsFolderChildren ;
					foreach(const QString & newCategory, newCategories.keys())
					{
						if(newCategories[newCategory].size() == 1)  // lonely item case -> no new category but retain with current parent:
						{
							// retain lonely children:
							thisParentsDirectChildren.second << newCategories[newCategory] ;
							continue ;
						}
						// otherwise:  really create new category
						specFolderItem* newFolder = new specFolderItem(0, newCategory) ;
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
			emit progressValue(85 * descriptors.indexOf(descriptor) / descriptors.size());
		}
		// append final round's outcome to move list
		finalPositions << folderQueue ;

		emit progressValue(90);
		// now move on to creating the undo commands:
		Command = new specMultiCommand ;
		Command->setText(tr("Form tree of items")) ;
		Command->setParentObject(model) ;

		// add the new tree:
		int row = 0 ;
		while(toBeDeletedFolders.contains(currentItem) || !currentItem->isFolder())   // make sure tree won't be removed accidentally
		{
			row = currentItem->parent()->childNo(currentItem) ;
			currentItem = currentItem->parent() ;
		}
		if(! model->insertItems(QList<specModelItem*>() << treeRoot, model->index(currentItem), row))
		{
			cleanUp();
			return ;
		}

		specAddFolderCommand* insertTree = new specAddFolderCommand(Command) ;
		insertTree->setParentObject(model) ;
		insertTree->setItem(treeRoot);
		treeRoot = 0 ;

		// move items into the new tree:
		specMoveCommand* moveCommand = new specMoveCommand(Command) ;
		moveCommand->setParentObject(model) ;
		foreach(destination d, finalPositions)
		{
			if(d.second.isEmpty()) continue ;
			QModelIndexList indexes = model->indexList(d.second) ;
			moveCommand->setItems(indexes, model->index(d.first), d.first->children());
		}

		// delete superfluous folders:  TODO: connections???
		specDeleteCommand* deleteOldFolders = new specDeleteCommand(Command) ;
		deleteOldFolders->setParentObject(model) ;
		deleteOldFolders->setItems(toBeDeletedFolders) ;
		if(cleanUp()) return ;
		emit progressValue(100);
	}

	specUndoCommand* command()
	{
		specUndoCommand* c = Command ;
		Command = 0 ;
		return c ;
	}

	treeActionThread(specModel* Model, const itemPointerList& Items, const itemPointerList& Folders, specModelItem* CurrentItem)
		: specWorkerThread(100),
		  Command(0),
		  treeRoot(0),
		  model(Model),
		  currentItem(CurrentItem),
		  items(Items),
		  toBeDeletedFolders(Folders)
	{
	}
};

specTreeAction::specTreeAction(QObject* parent) :
	specRequiresItemAction(parent)
{
	setIcon(QIcon(":/tree.png")) ;
	setToolTip(tr("Set up a directory tree")) ;
	setWhatsThis(tr("Generates a tree of directories and moves the selected items into those directories.  The columns in this dock window's list will be used from left to right to establish the level of folders within the tree."));
	setText(tr("Build directory tree")) ;
	setShortcut(tr("Ctrl+T"));
}


specUndoCommand* specTreeAction::generateUndoCommand()
{
	QList<specModelItem*> items, folders ;
	expandSelectedFolders(items, folders) ;
	treeActionThread tat(model, items, folders, currentItem) ;
	tat.run();
	return tat.command() ;
}
