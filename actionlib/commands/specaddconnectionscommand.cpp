#include "specaddconnectionscommand.h"
#include "specmetaitem.h"

specAddConnectionsCommand::specAddConnectionsCommand(specUndoCommand *parent)
	: specManageConnectionsCommand(parent)
{
}

void specAddConnectionsCommand::doIt()
{
	restore();
}

void specAddConnectionsCommand::undoIt()
{
	take();
}

void specAddConnectionsCommand::processServers(specMetaItem *client, QList<specModelItem *> &servers) const
{
	servers = (servers.toSet() - client->serverList().toSet()).toList() ;
}
