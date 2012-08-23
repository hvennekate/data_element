#include "specresizesvgcommand.h"
#include "specsvgitem.h"
#include "specgenealogy.h"

specResizeSVGcommand::specResizeSVGcommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specResizeSVGcommand::setItem(const QModelIndex &index)
{
	if (item)
		delete item ;
	item = new specGenealogy(QModelIndexList() << index) ;
	other = ((specSVGItem*) item->firstItem())->getBounds() ;
	doIt() ;
}

bool specResizeSVGcommand::ok()
{
	return item ; // maybe check validity of size
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
	specSVGItem *pointer = ((specSVGItem*) item->firstItem()) ;
	specSVGItem::bounds oldBounds = pointer->getBounds() ;
	pointer->setBounds(other);
	other = oldBounds ;
	if (pointer->plot())
		pointer->plot()->replot();
}

void specResizeSVGcommand::undoIt()
{
	doIt() ;
}

bool specResizeSVGcommand::mergeable(const specUndoCommand *other)
{
	return (*item == (*((specResizeSVGcommand*) other)->item)) ;
}

bool specResizeSVGcommand::mergeWith(const QUndoCommand *other)
{
	return mergeable((specUndoCommand*) other) ;
}

void specResizeSVGcommand::parentAssigned()
{
	if (!parentObject()) return ;
	specModel *model = qobject_cast<specModel*>(parentObject()) ;
	if (!model) return ;
	item->setModel(model) ;
}
