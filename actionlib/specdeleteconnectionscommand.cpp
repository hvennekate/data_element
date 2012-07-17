#include "specdeleteconnectionscommand.h"

specDeleteConnectionsCommand::specDeleteConnectionsCommand(specUndoCommand *parent)
	: specManageConnectionsCommand(parent)
{
}

void specDeleteConnectionsCommand::doIt()
{
	take() ;
}

void specDeleteConnectionsCommand::undoIt()
{
	restore() ;
}
