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

void specUndoCommand::readLabel(QDataStream &in)
{
	QString d ;
	in >> d ;
	setText(d) ;
}

void specUndoCommand::writeToStream(QDataStream& out) const
{
	out << description() ;
	writeCommand(out) ;
}

void specUndoCommand::readFromStream(QDataStream& in)
{
	readLabel(in) ;
	readCommand(in) ;
}

void specUndoCommand::readAlternative(QDataStream &in, specStreamable::type t)
{
	readLabel(in) ;
	readAlternativeCommand(in, t) ;
}

void specUndoCommand::readAlternativeCommand(QDataStream &in, specStreamable::type t)
{
	Q_UNUSED(in)
	Q_UNUSED(t)
}

QString specUndoCommand::description() const
{
	return text() ;
}

specModel* specUndoCommand::model() const
{
	return dynamic_cast<specModel*>(parentObject()) ;
}
