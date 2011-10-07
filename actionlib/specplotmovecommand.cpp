#include "specplotmovecommand.h"

specPlotMoveCommand::specPlotMoveCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}

void specPlotMoveCommand::redo()
{
	if (!items.valid()) return ;
	foreach(specCanvasItem* item, items.items())
	{
		item->moveXBy(shift) ;
		item->scaleBy(scale) ; // TODO prevent scaling by zero.
		item->moveYBy(offset) ;
		item->addToSlope(slope) ;
		item->refreshPlotData();
	}
}

void specPlotMoveCommand::undo()
{
	if (!items.valid()) return ;
	foreach(specCanvasItem* item, items.items())
	{
		item->addToSlope(-slope) ;
		item->moveYBy(-offset) ;
		item->scaleBy(1./scale) ;
		item->moveXBy(-shift) ;
		item->refreshPlotData();
	}
}

bool specPlotMoveCommand::merge(specPlotMoveCommand *other)
{
	if (other->items != this->items) return false ;
	slope += other->slope * scale ;
	offset+= other->offset * scale - other->slope * scale * shift ;
	scale *= other->scale ;
	shift += other->shift ;
	return true ;
}

void specPlotMoveCommand::setItem(QModelIndex& index)
{
	item = specGenealogy(QModelIndexList() << index) ;
}

void specPlotMoveCommand::setCorrections(double xShift, double yOffset, double ySlope, double yScale)
{
	slope = ySlope ;
	offset = yOffset ;
	shift = xShift ;
	scale = yScale ;
}
