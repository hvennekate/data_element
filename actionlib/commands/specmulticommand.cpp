#include "specmulticommand.h"
#include "specactionlibrary.h"

specMultiCommand::specMultiCommand(specUndoCommand* parent)
	: specUndoCommand(parent),
	  mayMerge(true)
{
}

void specMultiCommand::setMergeable(bool mergeable)
{
	mayMerge = mergeable ;
}

void specMultiCommand::doIt()
{
	QUndoCommand::redo() ;
}

void specMultiCommand::undoIt()
{
	QUndoCommand::undo() ;
}

void specMultiCommand::writeCommand(QDataStream &out) const
// TODO make read/write part of specUndoCommand
// +protected virtual worker functions
{
	out << mayMerge << qint32(childCount()) ;
	for (int i = 0 ; i < childCount() ; ++i)
		out << *((specUndoCommand*) child(i)) ;
}

void specMultiCommand::readCommand(QDataStream &in)
{
	qint32 children, id ;
	in >> mayMerge >> children ;
	for (int i = 0 ; i < children ; ++i)
	{

		in >> id ; // TODO throw if id does not match
		specUndoCommand *command = specActionLibrary::commandById(id,this) ;
		in >> *command ;
	}
}

bool specMultiCommand::mergeWith(const QUndoCommand *other)
{
	if (!mayMerge)
		return false ;
//	const specMultiCommand *command = (const specMultiCommand*) other ;
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

/*void specMultiCommand::parentAssigned() // TODO change calling code to not explicitly set every child's parent widget!
{
	specUndoCommand *childPointer = 0 ;
	for (int i = 0 ; i < childCount() ; ++i)
		if (childPointer = dynamic_cast<specUndoCommand*>(child(i)))
			childPointer->setParentObject(parentObject()) ;
}*/
