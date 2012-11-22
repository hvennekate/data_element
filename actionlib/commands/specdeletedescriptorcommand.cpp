#include "specdeletedescriptorcommand.h"
#include "specmodel.h"

specDeleteDescriptorCommand::specDeleteDescriptorCommand(specUndoCommand *parent,QString k)
    : specUndoCommand(parent),
      key(k)
{
}

void specDeleteDescriptorCommand::writeCommand(QDataStream &out) const
{
    out << key << contents ;
}

void specDeleteDescriptorCommand::readCommand(QDataStream &in)
{
    in >> key >> contents ;
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
DELETEDESCRIPTORFUNCTIONMACRO(undoIt, QListIterator<specDescriptor> it(contents) ; myModel->restoreDescriptor(it, key))
DELETEDESCRIPTORFUNCTIONMACRO(parentAssigned, if(contents.isEmpty()) myModel->dumpDescriptor(contents, key))
