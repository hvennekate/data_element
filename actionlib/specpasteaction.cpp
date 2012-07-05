#include "specpasteaction.h"
#include "specview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

#include <QApplication>
#include <QClipboard>

specPasteAction::specPasteAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-paste")) ;
}

const std::type_info &specPasteAction::possibleParent()
{
	return typeid(specView) ; // TODO change in other actions to more generic partners (i.e. specView instead of specDataView)
}

void specPasteAction::execute()
{
	specView *currentView = (specView*) parent() ; // TODO change in other actions to more generic partners (i.e. specView instead of specDataView)
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
	int count = model->itemPointer(index)->children() ;
	if (! model->dropMimeData(QApplication::clipboard()->mimeData(),Qt::CopyAction,row,0,index)) return ;
	count = model->itemPointer(index)->children() - count ; // now we know, how many were inserted...
	specAddFolderCommand *command = new specAddFolderCommand ;
	QModelIndexList list ;
	for (int i = 0 ; i < count ; ++i)
		list << model->index(i+row,0,index) ;
	command->setItems(list) ;

	command->setParentObject((QWidget*)parent()) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
