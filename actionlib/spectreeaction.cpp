#include "spectreeaction.h"
#include "specmulticommand.h"
#include "specaddfoldercommand.h"
#include "specmovecommand.h"

specTreeAction::specTreeAction(QObject *parent) :
	specUndoAction(parent)
{
	setIcon(QIcon(":/tree.png")) ;
}

void specTreeAction::execute()
{
	specView *view = (specView*) parentWidget() ;
	specModel *model = view->model() ;
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
		QString sortBy = headers[i] ; // always tree all folders except the ones produce by the current treeing step
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
	insertion->setParentObject(view) ;

	library->push(command);

	for (int i = 0 ; i < moveTargets.size() ; ++i)
	{
		if (moveTargets[i].second.isEmpty())
			continue ;
		QModelIndexList targets = model->indexList(moveTargets[i].second) ;
		QModelIndex parent = model->index(moveTargets[i].first) ;
		specMoveCommand *moveCommand = new specMoveCommand(targets,parent,0,command) ;
		moveCommand->setParentObject(view);
		moveCommand->redo();
	}
}
