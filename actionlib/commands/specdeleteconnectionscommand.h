#ifndef SPECDELETECONNECTIONSCOMMAND_H
#define SPECDELETECONNECTIONSCOMMAND_H

#include "specmanageconnectionscommand.h"

class specDeleteConnectionsCommand : public specManageConnectionsCommand
{
private:
	void doIt() ;
	void undoIt() ;
	type typeId() const { return specStreamable::deleteConnectionsCommandId ; }
	void processServers(specMetaItem* client, QList<specModelItem*>& servers) const ;
public:
	specDeleteConnectionsCommand(specUndoCommand* parent) ;
};

#endif // SPECDELETECONNECTIONSCOMMAND_H
