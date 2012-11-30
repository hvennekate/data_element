#include "specexchangedatacommand.h"
#include "qwt_plot.h"

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
    if (!item) return ;
    specDataItem *pointer = dynamic_cast<specDataItem*>(item->firstItem()) ;
    if (!pointer) return ;
    pointer->swapData(data);
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
