#ifndef SPECMANAGECONNECTIONSCOMMAND_H
#define SPECMANAGECONNECTIONSCOMMAND_H

#include "specmultipleitemcommand.h"

class specMetaModel ;

//// Change: This class is ONLY for connecting dataItems (servers) to a metaItem (client)
class specManageConnectionsCommand : public specMultipleItemCommand
{
private:
	specGenealogy target ;
	specMetaItem* targetPointer() ;
	specMetaModel* targetModel ;
	bool sameModel ;
	void writeCommand(QDataStream& out) const;
	void readCommand(QDataStream& in) ;
	void parentAssigned();
protected:
	virtual void processServers(specMetaItem* client, QList<specModelItem*>& servers) const = 0 ;
	void take() ;
	void restore() ;
public:
	explicit specManageConnectionsCommand(specUndoCommand* parent = 0);
	virtual void setItems(specMetaItem* client, QList<specModelItem*> servers) ;
};

#endif // SPECMANAGECONNECTIONSCOMMAND_H
