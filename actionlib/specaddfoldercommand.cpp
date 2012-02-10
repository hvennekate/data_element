#include "specaddfoldercommand.h"
#include <QDebug>

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	: specManageItemsCommand(parent)
{
}

void specAddFolderCommand::redo()
{
	qDebug("restoring addFolderAction") ;
	restore();
}

void specAddFolderCommand::undo()
{
	qDebug("undoing addFolderAction") ;
	take() ;
}
