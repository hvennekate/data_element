#include "specresizesvgcommand.h"
#include "specsvgitem.h"

specResizeSVGcommand::specResizeSVGcommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specResizeSVGcommand::setItem(const QModelIndex &index, const QRectF &rect)
{
	if (item)
		delete item ;
	item = new specGenealogy(QModelIndexList() << index) ;
	other = rect ;
}

bool specResizeSVGcommand::ok()
{
	return item && other.isValid() ;
}

void specResizeSVGcommand::writeToStream(QDataStream &out) const
{
	out << other << *item ;
}

void specResizeSVGcommand::readFromStream(QDataStream &in)
{
	if (!item) new specGenealogy ;
	in >> other >> *item ;
}

void specResizeSVGcommand::doIt()
{
	exchange();
}

void specResizeSVGcommand::undoIt()
{
	exchange();
}

void specResizeSVGcommand::exchange()
{
	specSVGItem *pointer = ((specSVGItem*) item->items().first()) ;
	QRectF old = pointer->boundingRect() ;
	pointer->setBoundingRect(other) ;
	other = old ;
	if (pointer->plot())
		pointer->plot()->replot();
}

bool specResizeSVGcommand::mergeable(const specUndoCommand *other)
{
	return (*item == (*((specResizeSVGcommand*) other)->item)) ;
}

bool specResizeSVGcommand::mergeWith(const QUndoCommand *other)
{
	return mergeable((specUndoCommand*) other) ;
}
