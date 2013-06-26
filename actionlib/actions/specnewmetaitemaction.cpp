#include "specnewmetaitemaction.h"
#include "kinetic/specmetaview.h"
#include "specmetamodel.h"
#include "specaddfoldercommand.h"
#include "specmetaitem.h"

specNewMetaItemAction::specNewMetaItemAction(QObject *parent) :
	specItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("document-new")) ;
	setToolTip(tr("New item"));
	setWhatsThis(tr("Create a new item in this dock window's list."));
	setText(tr("New meta item")) ;
	setShortcut(tr("Ctrl+Shift+N"));
}

const std::type_info &specNewMetaItemAction::possibleParent()
{
	return typeid(specMetaView) ;
}

specUndoCommand * specNewMetaItemAction::generateUndoCommand()
{
	specMetaView *view = qobject_cast<specMetaView*>(specItemAction::view) ;
	if (!view) return 0 ;
	int row = 0 ;
	if (!currentItem->isFolder())
	{
		row = currentIndex.row()+1 ;
		currentIndex = currentIndex.parent() ;
	}

	specMetaItem *pointer = new specMetaItem ;
	pointer->setModels(model,view->getDataView()->model());
	if (! model->insertItems(QList<specModelItem*>() << pointer, currentIndex,row))
	{
		delete pointer ;
		return 0;
	}
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(pointer)) ;
	command->setParentObject(model) ;
	command->setText(tr("Add meta item")) ;
	return command ;
}
