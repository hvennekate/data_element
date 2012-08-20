#include "specnewmetaitemaction.h"
#include "kinetic/specmetaview.h"
#include "specmetamodel.h"
#include "specaddfoldercommand.h"
#include "specmetaitem.h"

specNewMetaItemAction::specNewMetaItemAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("document-new")) ;
}

const std::type_info &specNewMetaItemAction::possibleParent()
{
	return typeid(specMetaView) ;
}

void specNewMetaItemAction::execute()
{
	if (!parent()) return ;
	specMetaView *view = (specMetaView*) parent() ;
	specMetaModel *model = view->model() ;
	QModelIndex index = view->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	if (!item->isFolder())
	{
		row = index.row()+1 ;
		index = index.parent() ;
	}

	specMetaItem *pointer = new specMetaItem ;
	pointer->setModels(model,view->getDataView()->model());
	if (! model->insertItems(QList<specModelItem*>() << pointer, index,row))
	{
		delete pointer ;
		return ;
	}
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(pointer)) ;
	command->setParentObject((QWidget*) parent()) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
