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

	int begin = 0, end = moveTargets.size() ;
	for (int i = 0 ; i < headers.size() ; ++i)
	{
		QString sortBy = headers[i] ;
		for (int j = begin ; j != end ; ++j)
			treeFolder(moveTargets[j].first,moveTargets[j].second,sortBy) ;
		begin = end ;
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

void specTreeAction::treeFolder(specFolderItem* folder, QModelIndexList& content, const QString &descriptor)
{
	qDebug() << "%%%%%% sorting by" << descriptor ;
	QMap<QString,QModelIndexList> subLists ;
	// create new lists of objects with equal descriptors
	for (int i = 0 ; i < content.size() ; ++i)
		subLists[model->itemPointer(content[i])->descriptor(descriptor,true)] << content[i] ;
	// remove lists with just one or all objects
	for (QMap<QString,QModelIndexList>::iterator i = subLists.begin() ; i != subLists.end() ; ++i)
		if (i.value().size() == 1 || (i.value().size() == content.size() && i+1 != subLists.end()))
			subLists.erase(i) ;
	// create new folders
	for (QMap<QString,QModelIndexList>::iterator i = subLists.begin() ; i != subLists.end() ; ++i)
	{
		for (int j = 0 ; j < i.value().size() ; ++j)
			content.removeAll(i.value()[j]) ;
		moveTargets << QPair<specFolderItem*,QModelIndexList>(new specFolderItem(folder,i.key()), i.value());
	}
}
