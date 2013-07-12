#ifndef SPECADDCONNECTIONSCOMMAND_H
#define SPECADDCONNECTIONSCOMMAND_H

#include "specmanageconnectionscommand.h"

class specAddConnectionsCommand : public specManageConnectionsCommand
{
private:
	void doIt();
	void undoIt();
	type typeId() const { return specStreamable::newConnectionsCommandId ; }
	void processServers(specMetaItem *client, QList<specModelItem *> &servers) const ;
public:
	specAddConnectionsCommand(specUndoCommand *parent = 0);
};

#endif // SPECADDCONNECTIONSCOMMAND_H
