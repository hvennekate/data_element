#include "specaddfolderaction.h"
#include "specdataview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

specAddFolderAction::specAddFolderAction(QObject *parent)
	: specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("folder-new"));
}


const std::type_info &specAddFolderAction::possibleParent()
{
	return typeid(specDataView) ;
}

void specAddFolderAction::execute()
{
	specDataView *currentView = (specDataView*) parent() ;
	specModel *model = currentView->model() ;
	QModelIndex index = currentView->currentIndex() ;
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
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(row,0,index)) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
