#include "specremovedatacommand.h"

specRemoveDataCommand::specRemoveDataCommand(specUndoCommand *parent)
	: specManageDataCommand(parent)
{
}

void specRemoveDataCommand::redo()
{
	take() ;
}

void specRemoveDataCommand::undo()
{
	insert();
}
