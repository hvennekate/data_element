#include "spectogglefitstylecommand.h"

specToggleFitStyleCommand::specToggleFitStyleCommand(specUndoCommand* parent)
    : specSingleItemCommand
#ifdef WIN32BUILD
	      <specMetaItem>
#endif
		(parent)
{
}

void specToggleFitStyleCommand::undoIt()
{
	doIt() ;
}

void specToggleFitStyleCommand::doIt()
{
	specMetaItem* pointer = itemPointer() ;
	if(!pointer) return ;
	pointer->toggleFitStyle() ;
}
