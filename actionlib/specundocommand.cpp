#include "specundocommand.h"

specUndoCommand::specUndoCommand(specUndoCommand *parent) :
    QUndoCommand(parent)
{
}

QDataStream& specUndoCommand::write(QDataStream& out)
{
	return out ;
}

QDataStream& specUndoCommand::read(QDataStream& in)
{
	return in ;
}


