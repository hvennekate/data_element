#include "specaddfoldercommand.h"

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	: specManageItemsCommand(parent)
{
}

void specAddFolderCommand::redo()
{
	restore();
}

void specAddFolderCommand::undo()
{
	take() ;
}
