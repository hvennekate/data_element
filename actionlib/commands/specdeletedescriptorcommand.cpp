#include "specdeletedescriptorcommand.h"
#include "specmodel.h"

specDeleteDescriptorCommand::specDeleteDescriptorCommand(specUndoCommand *parent,QString k)
    : specUndoCommand(parent),
      key(k),
      position(0)
{
}

void specDeleteDescriptorCommand::writeCommand(QDataStream &out) const
{
    out << position << (qint8) flags << key << contents ;
}

void specDeleteDescriptorCommand::readCommand(QDataStream &in)
{
	qint8 f ;
    in >> position >> f >> key >> contents ;
    flags = (spec::descriptorFlags) f ;
}

#define DELETEDESCRIPTORFUNCTIONMACRO(fname, code)     void specDeleteDescriptorCommand::fname() \
    { \
        specModel* myModel = qobject_cast<specModel*>(parentObject()) ; \
        if (!myModel) return ; \
        myModel->signalBeginReset(); \
        code ; \
        myModel->signalEndReset(); \
    }

DELETEDESCRIPTORFUNCTIONMACRO(doIt, myModel->deleteDescriptor(key))
DELETEDESCRIPTORFUNCTIONMACRO(undoIt, QListIterator<specDescriptor> it(contents) ; myModel->restoreDescriptor(position, flags, it, key))
DELETEDESCRIPTORFUNCTIONMACRO(parentAssigned, if(contents.isEmpty()) \
{ \
	myModel->dumpDescriptor(contents, key) ; \
	position = myModel->descriptors().indexOf(key) ; \
	flags = myModel->descriptorProperties()[position] ; \
})
