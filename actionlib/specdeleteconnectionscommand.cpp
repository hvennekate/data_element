#include "specdeleteconnectionscommand.h"

specDeleteConnectionsCommand::specDeleteConnectionsCommand(specUndoCommand *parent)
	: specManageConnectionsCommand(parent)
{
}

void specDeleteConnectionsCommand::redo()
{
	take() ;
}

void specDeleteConnectionsCommand::undo()
{
	restore() ;
}
