#include "specresizesvgcommand.h"
#include "specsvgitem.h"
#include "specgenealogy.h"
#include "qwt_plot.h"

specResizeSVGcommand::specResizeSVGcommand(specUndoCommand* parent)
	: specSingleItemCommand
#ifdef WIN32BUILD
		<specSVGItem>
#endif
	  (parent),
	  anchor(specSVGItem::undefined)
{
}

void specResizeSVGcommand::setItem(specModelItem* item, const specSVGItem::bounds& bounds, specSVGItem::SVGCornerPoint a)
{
	specSingleItemCommand
#ifdef WIN32BUILD
		<specSVGItem>
#endif
		::setItem(item) ;
	other = bounds ;
	anchor = a ;
	doIt() ;
}

void specResizeSVGcommand::writeCommand(QDataStream& out) const
{
	out << other ;
	writeItem(out) ;
	out << (qint8) anchor ;
}

void specResizeSVGcommand::readCommand(QDataStream& in)
{
	qint8 newAnchor ;
	in >> other ;
	readItem(in) ;
	in >> newAnchor ;
	anchor = (specSVGItem::SVGCornerPoint) newAnchor ;
}

void specResizeSVGcommand::doIt()
{
	specSVGItem* pointer = itemPointer() ;
	specSVGItem::bounds oldBounds = pointer->getBounds() ;
	pointer->setBounds(other);
	if(anchor != specSVGItem::undefined)
		anchor = pointer->setAnchor(anchor) ;
	other = oldBounds ;
	if(pointer->plot())
		pointer->plot()->replot();
}

void specResizeSVGcommand::undoIt()
{
	doIt() ;
}

bool specResizeSVGcommand::mergeable(const specUndoCommand* other)
{
	return (itemPointer() == ((specResizeSVGcommand*) other)->itemPointer()) ;
}

bool specResizeSVGcommand::mergeWith(const QUndoCommand* other)
{
	if(!parentObject()) return false ;
	return mergeable((specUndoCommand*) other) ;
}

