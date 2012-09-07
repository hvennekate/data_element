#include "specaddfolderaction.h"
#include "specdataview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

specAddFolderAction::specAddFolderAction(QObject *parent)
	: specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("folder-new"));
	setToolTip(tr("Create folder")) ;
	setWhatsThis(tr("Creates a new folder that may then be filled with items."));
}


const std::type_info &specAddFolderAction::possibleParent()
{
	return typeid(specView) ;
}

void specAddFolderAction::execute()
{
	specView *currentView = (specView*) parent() ;
	specModel *model = currentView->model() ;
	QModelIndex index = currentView->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	if (!item->isFolder())
	{
		row = index.row()+1 ;
		index = index.parent() ;

	}
	if (! model->insertItems(QList<specModelItem*>() << new specFolderItem(), index, row)) return ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(row,0,index)) ;

	command->setText(tr("Add folder")) ;
	command->setParentObject(model) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
