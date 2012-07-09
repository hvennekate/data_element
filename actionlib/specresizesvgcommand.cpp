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

void specResizeSVGcommand::write(specOutStream &out) const
{
	out.startContainer(spec::resizeSVGCommandId) ;
	out << other ;
	item->write(out) ;
	out.stopContainer();
}

bool specResizeSVGcommand::read(specInStream &in)
{
	if (!in.expect(spec::resizeSVGCommandId)) ;
	if (!parentObject()) return in ;
	specModel *model = ((specView*) parentObject())->model();
	if (!model) return in ; // TODO dynamic_cast
	in >> other ;
	if (item) delete item ;
	item = new specGenealogy(model, in) ;
	return !in.next() ;
}

void specResizeSVGcommand::redo()
{
	exchange();
}

void specResizeSVGcommand::undo()
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
