#include "specundocommand.h"
#include <QDebug>

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
	qDebug() << "!!!!!!! redoing command" << pO ;
	if (pO) doIt() ;
}

void specUndoCommand::undo()
{
	if (pO) undoIt();
}
