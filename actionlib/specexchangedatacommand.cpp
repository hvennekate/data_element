#include "specexchangedatacommand.h"

specExchangeDataCommand::specExchangeDataCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specExchangeDataCommand::setItem(const QModelIndex & index, const QVector<specDataPoint> &newData)
{
	data = newData ;
	if (!parentObject()) return ; // TODO dynamic cast
	specDataItem *pointer = dynamic_cast<specDataItem*>(((specView*) parentObject())->model()->itemPointer(index)) ;
	if (item)
		delete item ;
	item = 0 ;
	if (!pointer)
		return ;
	item = new specGenealogy(QModelIndexList() << index) ;
}

void specExchangeDataCommand::undoIt()
{
	doIt() ;
}

void specExchangeDataCommand::doIt()
{
	if (!item)
		return ;
	specDataItem *pointer = (specDataItem*) (item->items().first()) ;
	QVector<specDataPoint> currentData(pointer->allData()) ; // TODO introduce swap here via function of the data item
	pointer->setData(data);
	data = currentData ;
	if (pointer->plot())
		pointer->plot()->replot() ;
}

void specExchangeDataCommand::writeToStream(QDataStream &out) const
{
	out << data << *item ;
}

void specExchangeDataCommand::readFromStream(QDataStream &in)
{
	if (!item) item = new specGenealogy ;
	in >> data >> *item ;
}
