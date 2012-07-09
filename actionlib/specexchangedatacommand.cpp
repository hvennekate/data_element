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

void specExchangeDataCommand::undo()
{
	redo() ;
}

void specExchangeDataCommand::redo()
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

void specExchangeDataCommand::write(specOutStream &out) const
{
	out.startContainer(spec::exchangeDataCommandId);
	item->write(out << data) ;
	out.stopContainer();
}

bool specExchangeDataCommand::read(specInStream &in)
{
	if (!in.expect(spec::exchangeDataCommandId)) return false ;
	if (item) delete item ;
	in >> data ;
	item = new specGenealogy(((specView*) parentObject())->model(),in) ;
	return !in.next() ;
}
