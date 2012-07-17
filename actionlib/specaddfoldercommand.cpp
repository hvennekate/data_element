#include "specaddfoldercommand.h"
#include <QDebug>

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	: specManageItemsCommand(parent)
{
}

void specAddFolderCommand::doIt()
{
	qDebug("restoring addFolderAction") ;
	restore();
}

void specAddFolderCommand::undoIt()
{
	qDebug("undoing addFolderAction") ;
	take() ;
}
