#include "specmanageitemscommand.h"
#include <QAbstractItemView>

specManageItemsCommand::specManageItemsCommand(specUndoCommand *parent) :
	specMultipleItemCommand(parent)
{
}

void specManageItemsCommand::setItems(QList<specModelItem *> &pointers)
{
	specModel::eliminateChildren(pointers) ;
	qSort(pointers.begin(), pointers.end(), specModel::lessThanItemPointer) ;
	specMultipleItemCommand::setItems(pointers) ;
}

void specManageItemsCommand::setItem(specModelItem* pointer)
{
	setItems(QList<specModelItem*>() << pointer) ;
}
