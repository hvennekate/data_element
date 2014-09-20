#include "specdeletedescriptorcommand.h"
#include "specmodel.h"

specDeleteDescriptorCommand::specDeleteDescriptorCommand(specUndoCommand* parent, QString k)
	: specUndoCommand(parent),
	  key(k),
	  position(0)
{
}

void specDeleteDescriptorCommand::writeCommand(QDataStream& out) const
{
	out << position << key << contents ;
}

void specDeleteDescriptorCommand::readCommand(QDataStream& in)
{
	in >> position >> key >> contents ; // TODO remove flags
}

#define DELETEDESCRIPTORFUNCTIONMACRO(fname, code)     void specDeleteDescriptorCommand::fname() \
	{ \
		specModel* myModel = qobject_cast<specModel*>(parentObject()) ; \
		if (!myModel) return ; \
		myModel->signalBeginReset(); \
		code ; \
	}

DELETEDESCRIPTORFUNCTIONMACRO(doIt, \
			      if(contents.isEmpty()) \
			      { \
				      myModel->dumpDescriptor(contents, key) ; \
				      position = myModel->descriptors().indexOf(key) ; \
			      } \
			      myModel->deleteDescriptor(key))
DELETEDESCRIPTORFUNCTIONMACRO(undoIt, QListIterator<specDescriptor> it(contents) ; myModel->restoreDescriptor(position, it, key))
DELETEDESCRIPTORFUNCTIONMACRO(parentAssigned,)

bool specDeleteDescriptorCommand::alternativeType(type t) const
{
	return legacyDeleteDescriptorCommandId == t ;
}

void specDeleteDescriptorCommand::readAlternativeCommand(QDataStream &in, type t)
{
	if (t != legacyDeleteDescriptorCommandId) return ;
	qint8 f ;
	QList<specLegacyDescriptor> oldDescriptors ;
	in >> position >> f >> key >> oldDescriptors ;
	contents.clear();
	foreach(const specLegacyDescriptor& d, oldDescriptors)
		contents << d ;
}

