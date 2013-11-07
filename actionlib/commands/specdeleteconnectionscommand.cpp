#include "specdeleteconnectionscommand.h"
#include "specmetaitem.h"

specDeleteConnectionsCommand::specDeleteConnectionsCommand(specUndoCommand* parent)
	: specManageConnectionsCommand(parent)
{
}

void specDeleteConnectionsCommand::doIt()
{
	take() ;
}

void specDeleteConnectionsCommand::undoIt()
{
	restore() ;
}

void specDeleteConnectionsCommand::processServers(specMetaItem* client, QList<specModelItem*>& servers) const
{
	servers = (servers.toSet() & (client->serverList().toSet())).toList() ;
}
