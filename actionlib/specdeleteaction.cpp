#include "specdeleteaction.h"
#include "specdeletecommand.h"

specDeleteAction::specDeleteAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-delete"));
}

const std::type_info &specDeleteAction::possibleParent()
{
	return typeid(specDataView) ;
}



void specDeleteAction::execute()
{
	qDebug("starting action") ;
	specDataView *currentView = (specDataView*) parent() ;
	QModelIndexList indexes = currentView->selectionModel()->selectedIndexes() ;
	qDebug("=== getting selection") ;
	specModel *model = currentView->model() ;
	model->eliminateChildren(indexes) ;
	qDebug("=== got pointers") ;
	specDeleteCommand *command = new specDeleteCommand() ;
	command->setModel(model) ;
	foreach(QModelIndex index, indexes)
		command->addItem(model->itemPointer(index)) ;
	qDebug("=== prepared command") ;
	currentView->selectionModel()->clearSelection();
	qDebug("=== cleared selection") ;
	if (command->ok())
		library->push(command) ;
//		command->redo();
	qDebug("=== pushed command onto stack") ;
}
