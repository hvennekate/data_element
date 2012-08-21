#include "specresizesvgcommand.h"
#include "specsvgitem.h"

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

void specResizeSVGcommand::writeToStream(QDataStream &out) const
{
	out << other << *item << (qint8) anchor ;
}

void specResizeSVGcommand::readFromStream(QDataStream &in)
{
	qint8 newAnchor ;
	if (!item) new specGenealogy ;
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
	qDebug() << "Old bounds:" << oldBounds.x.second << oldBounds.y.second << oldBounds.width.second << oldBounds.height.second ;
	qDebug() << "New bounds:" << other.x.second << other.y.second << other.width.second << other.height.second ;
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
