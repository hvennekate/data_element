#include "specdeletecommand.h"

specDeleteCommand::specDeleteCommand(specUndoCommand *parent)
	: specManageItemsCommand(parent)
{
}

void specDeleteCommand::redo()
{
	take();
}

void specDeleteCommand::undo()
{
	restore() ;
}
