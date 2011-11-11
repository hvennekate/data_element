#include "specmanagedatacommand.h"

specManageDataCommand::specManageDataCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specManageDataCommand::setItem(const QModelIndex& index, const QVector<int>& dataIndexes)
{
	specDataItem *pointer = dynamic_cast<specDataItem*>(specModel::itemPointer(index)) ;
	if (!pointer)
	{
		toTake.clear();
		taken.clear();
		if (item)
			delete item ;
		item = 0 ;
		return ;
	}

	item = new specGenealogy(QModelIndexList() << index) ;
	toTake = dataIndexes ;
	taken  = pointer->getData(toTake) ;
}

void specManageDataCommand::take()
{
	if (!item)
		return ;
	((specDataItem*) (item->items().first()))->removeData(toTake) ;
}

void specManageDataCommand::insert()
{
	if (!item)
		return ;
	((specDataItem*) (item->items().first()))->insertData(taken) ;
}

QDataStrem& specManageDataCommand::write(QDataStream &out) const
{
	return item->write(out) << toTake << taken ;
}

QDataStream& specManageDataCommand::read(QDataStream &in)
{
	if (item)
		delete item ;
	item = new specGenealogy(((specView*) parentWidget())->model(),in) ;
	return in >> toTake >> taken ;
}

bool specManageDataCommand::ok()
{
	if (!item)
		return false ;
	return item->valid() && toTake.size() == taken.size() && !toTake.isEmpty() ;
}
