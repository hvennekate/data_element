#include "spectreeaction.h"
#include "specmulticommand.h"
#include "specaddfoldercommand.h"
#include "specmovecommand.h"

specTreeAction::specTreeAction(QObject *parent) :
	specUndoAction(parent)
{
	setIcon(QIcon(":/tree.png")) ;
	setToolTip(tr("Set up a directory tree")) ;
	setWhatsThis(tr("Generates a tree of directories and moves the selected items into those directories.  The columns in this dock window's list will be used from left to right to establish the level of folders within the tree."));
}


void specTreeAction::execute()
{
	specView *view = (specView*) parentWidget() ;
	if (view->getSelection().isEmpty()) return ; // Teil von 2. Bug von Daniel
	specModel *model = view->model() ;
	if (!model) return ;
	QVector<QPair<specFolderItem*,QList<specModelItem*> > > moveTargets ;

	// getting header strings
	int columnCount = model->columnCount(QModelIndex()) ;
	QStringList headers ;
	for (int i = 0 ; i < columnCount ; ++i)
		headers << model->headerData(i,Qt::Horizontal).toString() ;

	// preparing new tree directories
	moveTargets << QPair<specFolderItem*,QList<specModelItem*> >(new specFolderItem(),model->pointerList(view->getSelection())) ;

	for (int i = 0 ; i < headers.size() ; ++i)
	{
		QString sortBy = headers[i] ; // always tree all folders except the ones produced by the current treeing step
		int end = moveTargets.size() ;
		for (int j = 0 ; j != end ; ++j)
		{
			specFolderItem *folder = moveTargets[j].first ; // folder to operate on
			QList<specModelItem*> items = moveTargets[j].second ; // item list to operatate on
			QMap<QString,QList<specModelItem*> > subLists ;
			// create new lists of objects with equal descriptors
			for (int k = 0 ; k < items.size() ; ++k)
				subLists[items[k]->descriptor(sortBy,true)] << items[k] ;
			// remove lists with just one or all objects (all objects => just one sublist)
			for (QMap<QString,QList<specModelItem*> >::iterator k = subLists.begin() ; k != subLists.end() ; ++k)
				if (k.value().size() == 1 || subLists.size() == 1)
					subLists.erase(k) ;
			// create new folders and remove their children from old parent
			for (QMap<QString,QList<specModelItem*> >::iterator k = subLists.begin() ; k != subLists.end() ; ++k)
			{
				for (int l = 0 ; l < k.value().size() ; ++l)
					items.removeAll(k.value()[l]) ;
				moveTargets << QPair<specFolderItem*, QList<specModelItem*> >(new specFolderItem(folder,k.key()), k.value()) ;
				folder->addChild(moveTargets.last().first,folder->children()) ;
			}
			// putting left-over items back to old parent
			moveTargets[j].second = items ;
		}
	}

	// TODO produce more generic code with addFolderAction
	QModelIndex index = view->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	if (!item->isFolder())
	{
		row = index.row()+1 ;
		index = index.parent() ;
	}
	if (! model->insertItems(QList<specModelItem*>() << moveTargets.first().first, index, row)) return ;
	specMultiCommand *command = new specMultiCommand ;
	command->setParentObject(view) ;
	command->setMergeable(false) ;

	specAddFolderCommand *insertion = new specAddFolderCommand(command) ;
	insertion->setItems(QModelIndexList() << model->index(row,0,index)) ;
	insertion->setParentObject(model) ;

	command->setText(tr("Generate item tree")) ;
	library->push(command);

	for (int i = 0 ; i < moveTargets.size() ; ++i)
	{
		if (moveTargets[i].second.isEmpty())
			continue ;
		QModelIndexList targets = model->indexList(moveTargets[i].second) ;
		QModelIndex parent = model->index(moveTargets[i].first) ;
		specMoveCommand *moveCommand = new specMoveCommand(command) ;
		moveCommand->setParentObject(view);
		moveCommand->setItems(targets,parent,0) ;
		moveCommand->redo();
	}
}

/*void specTreeAction::execute()
{
	specView *view = (specView*) parentWidget() ;
	if (view->getSelection().isEmpty()) return ; // Teil von 2. Bug von Daniel
	specModel *model = view->model() ;
	if (!model) return ;

	// eliminate folders
	QList<specModelItem*> items = model->pointerList(view->getSelection()) ;
	for (int i = 0 ; i < items.size() ; ++i)
	{
		if (items[i]->isFolder())
		{
			for (int j = 0 ; j < items[i]->children() ; ++j)
				items << ((specFolderItem*) (items[i]))->child(i) ;
			items.takeAt(i) ;
		}
	}

	// getting header strings
	int columnCount = model->columnCount(QModelIndex()) ;
	QStringList headers ;
	for (int i = 0 ; i < columnCount ; ++i)
		headers << model->headerData(i,Qt::Horizontal).toString() ;

	specModelItem::descriptorComparison comparison(&headers) ;
	qSort(items.begin(), items.end(), comparison) ;


}

*/
