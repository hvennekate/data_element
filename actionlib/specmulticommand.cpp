#include "specmulticommand.h"
#include "specactionlibrary.h"

specMultiCommand::specMultiCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}

void specMultiCommand::redo()
{
	QUndoCommand::redo() ;
}

void specMultiCommand::undo()
{
	QUndoCommand::undo() ;
}

QDataStream& specMultiCommand::write(QDataStream &out) const
{
	out << qint32(childCount()) ;
	for (int i = 0 ; i < childCount() ; ++i)
		((const specUndoCommand*) child(i))->write(out) ;
}

QDataStream& specMultiCommand::read(QDataStream &in)
{
	qint32 children, id ;
	in >> children ;

	for (int i = 0 ; i < children ; ++i)
	{
		in >> id ; // TODO throw if id does not match
		specUndoCommand *command = specActionLibrary::commandById(id,this) ;
		command->setParentWidget(parentWidget()) ;
		command->read(in) ;
	}
}

bool specMultiCommand::mergeWith(const QUndoCommand *other)
{
	const specMultiCommand *command = (const specMultiCommand*) other ;
	if (other->childCount() != childCount()) return false ;
	bool mergeable = true ;
	for (int i = 0 ; i < childCount() ; ++i)
	{
		if (child(i)->id() != other->child(i)->id())
			return false ;
		mergeable = mergeable && ((specUndoCommand*) child(i))->mergeable((specUndoCommand*) other->child(i)) ; // TODO do cast of both items first
	}
	// TODO maybe leave the check up to copy() function of child
	if (!mergeable) return false ;

	for (int i = 0 ; i < childCount() ; ++i)
		((specUndoCommand*) child(i))->mergeWith((const specUndoCommand*) other->child(i)) ;

	return true ;
}
