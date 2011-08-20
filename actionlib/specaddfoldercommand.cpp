#include "specaddfoldercommand.h"

specAddFolderCommand::specAddFolderCommand(specUndoCommand* parent)
	:specUndoCommand(parent)
{
}

void specAddFolderCommand::setPosition(specGenealogy *pos)
{
	position = pos ;
}

bool specAddFolderCommand::ok()
{
	return position->valid() ;
}

void specAddFolderCommand::redo()
{
	specModel *model = position->model() ;
	model->insertItems(QList<specModelItem*>() << position->pointer(),
			   position->parent(),
			   position->row()) ;

}

void specAddFolderCommand::undo()
{
	specModel *model = position->model() ;
	model->removeRow(position->row(),position->parent()) ;
}
