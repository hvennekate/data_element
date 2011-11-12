#include "specmanagedatacommand.h"

specManageDataCommand::specManageDataCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specManageDataCommand::setItem(const QModelIndex& index, const QVector<int>& dataIndexes)
{
	if (!parentWidget()) return ; // TODO dynamic cast
	specDataItem *pointer = dynamic_cast<specDataItem*>(((specView*) parentWidget())->model()->itemPointer(index)) ;
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
	specDataItem *pointer = ((specDataItem*) (item->items().first())) ;
	pointer->removeData(toTake) ;
	if (pointer->plot())
		pointer->plot()->replot();
}

void specManageDataCommand::insert()
{
	if (!item)
		return ;
	specDataItem *pointer = ((specDataItem*) (item->items().first())) ;
	pointer->insertData(taken) ;
	if (pointer->plot())
		pointer->plot()->replot();
}

QDataStream& specManageDataCommand::write(QDataStream &out) const
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
