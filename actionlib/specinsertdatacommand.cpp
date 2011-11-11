#include "specinsertdatacommand.h"

specInsertDataCommand::specInsertDataCommand(specUndoCommand *parent)
	: specManageDataCommand(parent)
{
}

void specInsertDataCommand::redo()
{
	insert() ;
}

void specInsertDataCommand::undo()
{
	take() ;
}
