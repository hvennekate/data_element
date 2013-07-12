#include "specdeletecommand.h"

specDeleteCommand::specDeleteCommand(specUndoCommand *parent)
	: specManageItemsCommand(parent)
{
}

void specDeleteCommand::doIt()
{
	takeItems();
}

void specDeleteCommand::undoIt()
{
	restoreItems(); ;
}
