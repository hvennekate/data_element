#include "spectogglefitstylecommand.h"

specToggleFitStyleCommand::specToggleFitStyleCommand(specUndoCommand *parent)
	: specSingleItemCommand(parent)
{
}

void specToggleFitStyleCommand::undoIt()
{
	doIt() ;
}

void specToggleFitStyleCommand::doIt()
{
	specMetaItem *pointer = itemPointer() ;
	if (!pointer) return ;
	pointer->toggleFitStyle() ;
}
