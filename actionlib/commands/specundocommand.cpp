#include "specundocommand.h"
#include "specmodel.h"

specUndoCommand::specUndoCommand(specUndoCommand* parent) :
	QUndoCommand(parent),
	pO(parent ? parent->parentObject() : 0)
{
}

void specUndoCommand::setParentObject(QObject* par)
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
	if(pO) doIt() ;
}

void specUndoCommand::undo()
{
	if(pO) undoIt();
}

void specUndoCommand::writeToStream(QDataStream& out) const
{
	out << description() ;
	writeCommand(out) ;
}

void specUndoCommand::readFromStream(QDataStream& in)
{
	QString d ;
	in >> d ;
	setText(d) ;
	readCommand(in) ;
}

QString specUndoCommand::description() const
{
	return text() ;
}

specModel* specUndoCommand::model() const
{
	return dynamic_cast<specModel*>(parentObject()) ;
}
