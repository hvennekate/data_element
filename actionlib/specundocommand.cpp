#include "specundocommand.h"

specUndoCommand::specUndoCommand(specUndoCommand *parent) :
    QUndoCommand(parent),
	pW(0) //parent->parentWidget()) // TODO adapt calling code
{
}

void specUndoCommand::setParentObject(QObject * par)
{
	pW = par ;
}

QObject* specUndoCommand::parentObject() const
{
	return pW ;
}
