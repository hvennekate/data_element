#include "specmetarangecommand.h"
#include "specmetaitem.h"

specMetaRangeCommand::specMetaRangeCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specMetaRangeCommand::doIt()
{
	if (!item) return ;
	item->seekParent() ;
	if (!item->valid()) return ;
	if (item->items().isEmpty()) return ;
	((specMetaItem*) item->items().first())->setRange(variableNo, rangeNo, pointNo, newX, newY) ;
	specModel* model ;
	if (parentObject() && (model = ((specView*) parentObject())->model()))
		model->signalChanged(model->index(item->items().first(), model->descriptors().indexOf("variables")));
}

void specMetaRangeCommand::undoIt()
{
	qSwap(newX, oldX) ;
	qSwap(newY, oldY) ;
	doIt() ;
	qSwap(newX, oldX) ;
	qSwap(newY, oldY) ;
}

bool specMetaRangeCommand::mergeWith(const QUndoCommand *ot)
{
	const specMetaRangeCommand* other = (const specMetaRangeCommand*) ot ;
	if (! (this->item && other->item)) return false ;
	if (*(other->item) != *item) return false ;
	if (other->variableNo != variableNo) return false ;
	if (other->rangeNo != rangeNo) return false ;
	if (other->pointNo != pointNo) return false ;
	newX = other->newX ;
	newY = other->newY ;
	return true ;
}

void specMetaRangeCommand::setItem(QModelIndex index, int variableIndex, int rangeIndex, int point, double nX, double nY)
{
	if (item) delete item ;
	item = new specGenealogy(QModelIndexList() << index) ;
	pointNo = point ;
	variableNo = variableIndex ;
	rangeNo = rangeIndex ;
	newX = nX ;
	newY = nY ;
	((specMetaItem*) (item->items().first()))->getRangePoint(variableNo, rangeNo, pointNo, oldX, oldY) ;
}

void specMetaRangeCommand::writeCommand(QDataStream &out) const
{ // TODO careful when writing pointer to genealogy.  Make local variable
	out << oldX << oldY << newX << newY << rangeNo << pointNo << variableNo << *item ;
}

void specMetaRangeCommand::readCommand(QDataStream &in)
{
	if (!item) item = new specGenealogy ;
	in >> oldX >> oldY >> newX >> newY >> rangeNo >> pointNo >> variableNo >> *item ;
}

bool specMetaRangeCommand::mergeable(const specUndoCommand *other)
{
	return *(((specMetaRangeCommand*) other)->item) == *item ;
}

void specMetaRangeCommand::parentAssigned()
{
	if (item) item->setModel(qobject_cast<specModel*>(parentObject()));
}
