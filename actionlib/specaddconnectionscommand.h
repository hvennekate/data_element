#ifndef SPECADDCONNECTIONSCOMMAND_H
#define SPECADDCONNECTIONSCOMMAND_H

#include "specmanageconnectionscommand.h"

class specAddConnectionsCommand : public specManageConnectionsCommand
{
public:
	specAddConnectionsCommand(specUndoCommand *parent = 0);
	void redo() ;
	void undo() ;
	int id() const { return spec::newConnectionsId ; }
};

#endif // SPECADDCONNECTIONSCOMMAND_H
