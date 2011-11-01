#include "spectreeaction.h"
#include "specmulticommand.h"
#include "specaddfoldercommand.h"
#include "specmovecommand.h"
#include <QDebug>

specTreeAction::specTreeAction(QObject *parent) :
	specUndoAction(parent)
{
}

void specTreeAction::execute()
{
	specMultiCommand *command = new specMultiCommand ;
	command->setParentWidget(view) ;

	view = (specView*) parentWidget() ;
	model = view->model() ;

	// prepare move hierarchy
	int columnCount = model->columnCount(QModelIndex()) ;
	QStringList headers ;
	for (int i = 0 ; i < columnCount ; ++i)
		headers << model->headerData(i,Qt::Horizontal).toString() ;
	moveTargets.clear();
	moveTargets << QPair<specFolderItem*,QModelIndexList>(new specFolderItem,view->getSelection()) ;
	qDebug() << "headers" << headers << "indexes:" << moveTargets ;

	int end = moveTargets.size() ;
	for (int i = 0 ; i < headers.size() ; ++i)
	{
		QString sortBy = headers[i] ;
		for (int j = 0 ; j != end ; ++j)
			treeFolder(j,sortBy) ;
		end = moveTargets.size() ;
	}

	// TODO produce more generic code with addFolderAction
	QModelIndex index = view->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	qDebug("checking if item is folder") ;
	if (!item->isFolder())
	{
		qDebug("item is not a folder") ;
		row = index.row()+1 ;
		index = index.parent() ;
	}
	if (! model->insertItems(QList<specModelItem*>() << new specFolderItem(), index, row)) return ;
	specAddFolderCommand *insertion = new specAddFolderCommand(command) ;
	insertion->setItems(QModelIndexList() << model->index(row,0,index)) ;
	insertion->setParentWidget((QWidget*)parent()) ;
	qDebug() << moveTargets ;

	for (int i = 0 ; i < moveTargets.size() ; ++i)
		new specMoveCommand(moveTargets[i].second,model->index(moveTargets[i].first),0,command) ;

	library->push(command);
}

void specTreeAction::treeFolder(int index, const QString &descriptor)
{
	specFolderItem *folder = moveTargets[index].first ;
	QModelIndexList& sourceList = moveTargets[index].second ;
	qDebug() << "%%%%%% sorting by" << descriptor ;
	QMap<QString,QModelIndexList> subLists ;
	// create new lists of objects with equal descriptors
	for (int i = 0 ; i < sourceList.size() ; ++i)
		subLists[model->itemPointer(sourceList[i])->descriptor(descriptor,true)] << sourceList[i] ;
	qDebug() << "created new lists" << subLists ;
	// remove lists with just one or all objects
	for (QMap<QString,QModelIndexList>::iterator i = subLists.begin() ; i != subLists.end() ; ++i)
		if (i.value().size() == 1 || subLists.size() == 1)
			subLists.erase(i) ;
	qDebug() << "removed superfluous lists" << subLists ;
	// create new folders
	for (QMap<QString,QModelIndexList>::iterator i = subLists.begin() ; i != subLists.end() ; ++i)
	{
		for (int j = 0 ; j < i.value().size() ; ++j)
			moveTargets[index].second.removeAt(sourceList.indexOf(i.value()[j])) ;
		moveTargets << QPair<specFolderItem*,QModelIndexList>(new specFolderItem(folder,i.key()), i.value());
	}
	qDebug() << "created new folders" << subLists ;
}
