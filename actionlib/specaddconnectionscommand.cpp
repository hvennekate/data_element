#include "specaddconnectionscommand.h"

specAddConnectionsCommand::specAddConnectionsCommand(specUndoCommand *parent)
	: specManageConnectionsCommand(parent)
{
}

void specAddConnectionsCommand::doIt()
{
	restore();
}

void specAddConnectionsCommand::undoIt()
{
	take();
}
