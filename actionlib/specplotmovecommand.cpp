#include "specplotmovecommand.h"

specPlotMoveCommand::specPlotMoveCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}

void specPlotMoveCommand::redo()
{
	foreach(specCanvasItem* item, items.items())
	{
		item->moveXBy(shift) ;
		item->scaleBy(scale) ;
		item->moveYBy(offset) ;
		item->addToSlope(slope) ;
	}
}

void specPlotMoveCommand::undo()
{
	foreach(specCanvasItem* item, items.items())
	{
		item->addToSlope(-slope) ;
		item->moveYBy(-offset) ;
		item->scaleBy(1./scale) ;
		item->moveXBy(-shift) ;
	}
}
