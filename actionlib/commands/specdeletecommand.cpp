#include "specdeletecommand.h"

specDeleteCommand::specDeleteCommand(specUndoCommand *parent)
	: specManageItemsCommand(parent)
{
}

void specDeleteCommand::doIt()
{
	take();
}

void specDeleteCommand::undoIt()
{
	restore() ;
}
