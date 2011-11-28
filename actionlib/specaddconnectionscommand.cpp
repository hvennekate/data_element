#include "specaddconnectionscommand.h"

specAddConnectionsCommand::specAddConnectionsCommand(specUndoCommand *parent)
	: specManageConnectionsCommand(parent)
{
}

void specAddConnectionsCommand::redo()
{
	restore();
}

void specAddConnectionsCommand::undo()
{
	take();
}
