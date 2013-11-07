#include "specmetarangecommand.h"
#include "specmetaitem.h"

specMetaRangeCommand::specMetaRangeCommand(specUndoCommand* parent)
	: specSingleItemCommand(parent)
{
}

void specMetaRangeCommand::doIt()
{
	itemPointer()->setRange(variableNo, rangeNo, pointNo, newX, newY) ;
	specModel* model = qobject_cast<specModel*> (parentObject());
	if(model)
		model->signalChanged(model->index(itemPointer(), model->descriptors().indexOf("variables")));
}

void specMetaRangeCommand::undoIt()
{
	qSwap(newX, oldX) ;
	qSwap(newY, oldY) ;
	doIt() ;
	qSwap(newX, oldX) ;
	qSwap(newY, oldY) ;
}

bool specMetaRangeCommand::mergeWith(const QUndoCommand* ot)
{
	if(!parentObject()) return false ;
	const specMetaRangeCommand* other = (const specMetaRangeCommand*) ot ;
	if(!(this->itemPointer() && other->itemPointer())) return false ;
	if(other->variableNo != variableNo) return false ;
	if(other->rangeNo != rangeNo) return false ;
	if(other->pointNo != pointNo) return false ;
	newX = other->newX ;
	newY = other->newY ;
	return true ;
}

void specMetaRangeCommand::setItem(specMetaItem* i, int variableIndex, int rangeIndex, int point, double nX, double nY)
{
	specSingleItemCommand::setItem(i) ;
	pointNo = point ;
	variableNo = variableIndex ;
	rangeNo = rangeIndex ;
	newX = nX ;
	newY = nY ;
	i->getRangePoint(variableNo, rangeNo, pointNo, oldX, oldY) ;
}

void specMetaRangeCommand::writeCommand(QDataStream& out) const
{
	// TODO careful when writing pointer to genealogy.  Make local variable
	out << oldX << oldY << newX << newY << rangeNo << pointNo << variableNo ;
	writeItem(out) ;
}

void specMetaRangeCommand::readCommand(QDataStream& in)
{
	in >> oldX >> oldY >> newX >> newY >> rangeNo >> pointNo >> variableNo ;
	readItem(in);
}

bool specMetaRangeCommand::mergeable(const specUndoCommand* other)
{
	return (((specMetaRangeCommand*) other)->itemPointer()) == itemPointer() ;
}
