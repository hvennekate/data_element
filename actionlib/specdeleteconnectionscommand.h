#ifndef SPECDELETECONNECTIONSCOMMAND_H
#define SPECDELETECONNECTIONSCOMMAND_H

#include "specmanageconnectionscommand.h"

class specDeleteConnectionsCommand : public specManageConnectionsCommand
{
public:
	specDeleteConnectionsCommand(specUndoCommand *parent) ;
	void redo() ;
	void undo() ;
};

#endif // SPECDELETECONNECTIONSCOMMAND_H
