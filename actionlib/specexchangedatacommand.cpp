#include "specexchangedatacommand.h"

specExchangeDataCommand::specExchangeDataCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specExchangeDataCommand::setItem(const QModelIndex & index, const QList<specDataPoint> &newData)
{
	data = newData ;
	if (!parentWidget()) return ; // TODO dynamic cast
	specDataItem *pointer = dynamic_cast<specDataItem*>(((specView*) parentWidget())->model()->itemPointer(index)) ;
	if (item)
		delete item ;
	item = 0 ;
	if (!pointer)
		return ;
	item = new specGenealogy(QModelIndexList() << index) ;
}

void specExchangeDataCommand::undo()
{
	redo() ;
}

void specExchangeDataCommand::redo()
{
	if (!item)
		return ;
	specDataItem *pointer = (specDataItem*) (item->items().first()) ;
	QList<specDataPoint> currentData(pointer->allData()) ; // TODO introduce swap here via function of the data item
	pointer->clearData();
	pointer->insertData(data) ;
	data = currentData ;
	if (pointer->plot())
		pointer->plot()->replot() ;
}

QDataStream& specExchangeDataCommand::write(QDataStream &out) const
{
	return item->write(out << data) ;
}

QDataStream& specExchangeDataCommand::read(QDataStream &in)
{
	if (item)
		delete item ;
	in >> data ;
	item = new specGenealogy(((specView*) parentWidget())->model(),in) ;
	return in ;
}
