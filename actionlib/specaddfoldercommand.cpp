#include "specaddfoldercommand.h"

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	:specUndoCommand(parent)
{
}


void specAddFolderCommand::setModel(specModel *mod)
{
	model = mod ;
}

bool specAddFolderCommand::ok()
{
	if (model) return true;
	return false ;
}

void specAddFolderCommand::redo()
{
	// create genealogy object!
}

void specAddFolderCommand::undo()
{
}
