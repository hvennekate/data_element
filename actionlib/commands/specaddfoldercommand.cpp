#include "specaddfoldercommand.h"

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	: specManageItemsCommand(parent)
{
}

void specAddFolderCommand::doIt()
{
	restoreItems();
}

void specAddFolderCommand::undoIt()
{
	takeItems();
}
