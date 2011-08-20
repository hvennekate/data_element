#include "specaddfolderaction.h"
#include "specdataview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"

specAddFolderAction::specAddFolderAction(QObject *parent) :
    specUndoAction(parent)
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
	specModelItem *item = model->itemPointer(currentView->currentIndex()) ;
	specGenealogy *position = ( item->isFolder() ?
					   new specGenealogy(new specFolderItem(),item->children(),item,model) :
					   new specGenealogy(new specFolderItem(),item->parent()->childNo(item),item->parent(),model)) ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setPosition(position) ;
	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
