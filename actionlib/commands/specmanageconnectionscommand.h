#ifndef SPECMANAGECONNECTIONSCOMMAND_H
#define SPECMANAGECONNECTIONSCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specManageConnectionsCommand : public specUndoCommand
{
private:
	QVector<specGenealogy*> items ;
	specGenealogy* target ;
	void clear() ;
	QVector<specModelItem*> itemPointers() const ;
	specMetaItem* targetPointer() const ;
	bool sameModel(specGenealogy*) const ;
	void writeToStream(QDataStream &out) const;
	void readFromStream(QDataStream &in) ;
	void parentAssigned();
protected:
	void take() ;
	void restore() ;
public:
	explicit specManageConnectionsCommand(specUndoCommand *parent = 0);
	virtual ~specManageConnectionsCommand() ;
	bool ok() const ;
	void setItems(const QModelIndex& client, QModelIndexList& servers) ;
};

#endif // SPECMANAGECONNECTIONSCOMMAND_H
