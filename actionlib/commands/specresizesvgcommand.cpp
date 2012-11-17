#include "specresizesvgcommand.h"
#include "specsvgitem.h"
#include "specgenealogy.h"
#include "qwt_plot.h"

specResizeSVGcommand::specResizeSVGcommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0),
	  anchor(specSVGItem::undefined)
{
}

void specResizeSVGcommand::setItem(const QModelIndex &index, const specSVGItem::bounds &bounds, specSVGItem::SVGCornerPoint a)
{
	if (item)
		delete item ;
	item = new specGenealogy(QModelIndexList() << index) ;
	other = bounds ;
	anchor = a ;
	doIt() ;
}

bool specResizeSVGcommand::ok()
{
	return item ; // maybe check validity of size
}

void specResizeSVGcommand::writeCommand(QDataStream &out) const
{
	out << other << *item << (qint8) anchor ;
}

void specResizeSVGcommand::readCommand(QDataStream &in)
{
	qint8 newAnchor ;
    if (!item) item = new specGenealogy ;
	in >> other >> *item >> newAnchor ;
	anchor = (specSVGItem::SVGCornerPoint) newAnchor ;
}

void specResizeSVGcommand::doIt()
{
	specSVGItem *pointer = ((specSVGItem*) item->firstItem()) ;
	specSVGItem::bounds oldBounds = pointer->getBounds() ;
	pointer->setBounds(other);
	if (anchor != specSVGItem::undefined)
		anchor = pointer->setAnchor(anchor) ;
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
    if (!parentObject()) return false ;
	return mergeable((specUndoCommand*) other) ;
}

void specResizeSVGcommand::parentAssigned()
{
	if (!parentObject()) return ;
	specModel *model = qobject_cast<specModel*>(parentObject()) ;
	if (!model) return ;
	if (item)
		item->setModel(model) ;
}
