#include "specmultiplotmovecommand.h"
#include "specplotmovecommand.h"

specMultiPlotMoveCommand::specMultiPlotMoveCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}


void specMultiPlotMoveCommand::redo()
{
	QUndoCommand::redo() ;
}

void specMultiPlotMoveCommand::undo()
{
	QUndoCommand::undo() ;
}

bool specMultiPlotMoveCommand::mergeWith(const QUndoCommand *other)
{
	const specMultiPlotMoveCommand* command = (const specMultiPlotMoveCommand*) other ;
	if (other->childCount() != childCount()) return false ;
	bool itemsMatch = true ;
	for (int i = 0 ; i < childCount() ; ++i)
		itemsMatch == itemsMatch && ((specPlotMoveCommand*) child(i))->itemsMatch((specPlotMoveCommand*) other->child(i)) ; // TODO do cast of both items first
	// TODO maybe leave the check up to copy() function of child
	if (!itemsMatch) return false ;

	for (int i = 0 ; i < childCount() ; ++i)
		((specPlotMoveCommand*) child(i))->mergeWith((const specPlotMoveCommand*) other->child(i)) ;

	return true ;
}

QDataStream& specMultiPlotMoveCommand::write(QDataStream &out) const
{
	out << qint32(childCount()) ;
	for (int i = 0 ; i < childCount() ; ++i)
		((const specUndoCommand*) child(i))->write(out) ;
}

QDataStream& specMultiPlotMoveCommand::read(QDataStream &in)
{
	qint32 children, id ;
	in >> children ;

	for (int i = 0 ; i < children ; ++i)
	{
		in >> id ; // TODO throw if id does not match
		specPlotMoveCommand *command = new specPlotMoveCommand(this) ;
		command->setParentWidget(parentWidget()) ;
		command->read(in) ;
	}
}
