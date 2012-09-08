#include "specaddfoldercommand.h"

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	: specManageItemsCommand(parent)
{
}

void specAddFolderCommand::doIt()
{
	restore();
}

void specAddFolderCommand::undoIt()
{
	take() ;
}
