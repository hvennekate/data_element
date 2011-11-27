#include "specnewmetaitemaction.h"
#include "kinetic/specmetaview.h"
#include "kinetic/specdescriptordescriptorfilter.h"
#include "specmetamodel.h"
#include "specaddfoldercommand.h"

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
	QModelIndex index = currentView->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	qDebug() << "checking if item is folder" << item << index ;
	if (!item->isFolder())
	{
		qDebug("item is not a folder") ;
		row = index.row()+1 ;
		index = index.parent() ;

	}

	specMetaItem *item = new specMetaItem ;
	specDescriptorDescriptorFilter *filter = new specDescriptorDescriptorFilter(item) ;
	filter->setXDescriptor("Zeit") ;
	filter->setYDescriptor("nu") ;
	if (! model->insertItems(QList<specModelItem*>() << item, index,row))
	{
		delete item ;
		return ;
	}
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(item)) ;
	command->setParentWidget((QWidget*) parent()) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
