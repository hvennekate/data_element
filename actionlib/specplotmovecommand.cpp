#include "specplotmovecommand.h"

specPlotMoveCommand::specPlotMoveCommand(specUndoCommand* parent)
	: specUndoCommand(parent),
	  items(0)
{
}

void specPlotMoveCommand::doIt()
{
	if (!items) return ;
	items->seekParent() ;
	if (!items->valid()) return ;
	qDebug("correcting items") ;
	foreach(specCanvasItem* item, items->items())
	{
		qDebug("applying corrections from undo command") ;
		item->moveXBy(shift) ;
		item->scaleBy(scale) ; // TODO prevent scaling by zero.
		item->moveYBy(offset) ;
		item->addToSlope(slope) ;
	}
}

void specPlotMoveCommand::undoIt()
{
	qDebug("undoing plot move command") ;
	if (!items) return ;
	items->seekParent() ;
	if (!items->valid()) return ;
	qDebug("starting to undo...") ;
	foreach(specCanvasItem* item, items->items())
	{
		item->addToSlope(-slope) ;
		item->moveYBy(-offset) ;
		item->scaleBy(1./scale) ;
		item->moveXBy(-shift) ;
	}
}


bool specPlotMoveCommand::mergeWith(const QUndoCommand* ot)
{
	qDebug("trying merge of move commands") ;
	const specPlotMoveCommand *other = (const specPlotMoveCommand*) ot ;
	qDebug("merging...") ;
	if (! (this->items && other->items)) return false ;
	if (*(other->items) != *(this->items)) return false ;
	slope += other->slope * scale ;
	offset+= other->offset * scale - other->slope * scale * shift ;
	scale *= other->scale ;
	shift += other->shift ;
	return true ;
}

void specPlotMoveCommand::setItem(QModelIndex index)
{
	if (items) delete items ;
	items = new specGenealogy(QModelIndexList() << index) ;
}

void specPlotMoveCommand::setCorrections(double xShift, double yOffset, double ySlope, double yScale)
{
	slope = ySlope ;
	offset = yOffset ;
	shift = xShift ;
	scale = yScale ;
}

void specPlotMoveCommand::writeToStream(QDataStream &out) const
{
	out << slope << offset << scale << shift << *items ;
}

void specPlotMoveCommand::readFromStream(QDataStream &in)
{
	if (!items) items = new specGenealogy ;
	in >> slope >> offset >> scale >> shift >> *items ;
}

bool specPlotMoveCommand::mergeable(const specUndoCommand *other)
{
	return *(((specPlotMoveCommand*) other)->items) == *items ;
}
