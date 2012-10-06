#include "specexchangedatacommand.h"

specExchangeDataCommand::specExchangeDataCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specExchangeDataCommand::setItem(const QModelIndex & index, const QVector<specDataPoint> &newData)
{
	data = newData ;
	if (!parentObject()) return ; // TODO dynamic cast/ obtain model from index
	specDataItem *pointer = dynamic_cast<specDataItem*>(((specModel*) parentObject())->itemPointer(index)) ;
	if (item)
		delete item ;
	item = 0 ;
	if (!pointer)
		return ;
	item = new specGenealogy(QModelIndexList() << index) ;
}

void specExchangeDataCommand::parentAssigned()
{
	if (!parentObject()) return ;
	if (!item) return ;
	item->setModel(qobject_cast<specModel*>(parentObject())) ;
}

void specExchangeDataCommand::undoIt()
{
	doIt() ;
}

void specExchangeDataCommand::doIt()
{
	if (!item)
		return ;
	specDataItem *pointer = (specDataItem*) (item->firstItem()) ;
	QVector<specDataPoint> currentData(pointer->allData()) ; // TODO introduce swap here via function of the data item
	pointer->setData(data);
	data = currentData ;
	if (pointer->plot()) // TODO is this really necessary?
		pointer->plot()->replot() ;
}

void specExchangeDataCommand::writeCommand(QDataStream &out) const
{
	out << data << *item ;
}

void specExchangeDataCommand::readCommand(QDataStream &in)
{
	if (!item) item = new specGenealogy ;
	in >> data >> *item ;
}
