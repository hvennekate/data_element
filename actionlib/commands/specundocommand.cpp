#include "specundocommand.h"

specUndoCommand::specUndoCommand(specUndoCommand *parent) :
    QUndoCommand(parent),
	pO(0) //parent->parentWidget()) // TODO adapt calling code
{
}

void specUndoCommand::setParentObject(QObject * par)
{
	pO = par ;
	parentAssigned();
}

QObject* specUndoCommand::parentObject() const
{
	return pO ;
}

void specUndoCommand::redo()
{
	if (pO) doIt() ;
}

void specUndoCommand::undo()
{
	if (pO) undoIt();
}

void specUndoCommand::writeToStream(QDataStream &out) const
{
	out << text() ;
	writeCommand(out) ;
}

void specUndoCommand::readFromStream(QDataStream &in)
{
	QString d ;
	in >> d ;
	setText(d) ;
	readCommand(in) ;
}
