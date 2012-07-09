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
	bool sameModel() const ;
protected:
	void take() ;
	void restore() ;
public:
	explicit specManageConnectionsCommand(specUndoCommand *parent = 0);
	virtual ~specManageConnectionsCommand() ;
	bool ok() const ;
	void setItems(const QModelIndex& client, QModelIndexList& servers) ;
	void write(QDataStream &out) const ;
	bool read(specInStream &in) ;
};

#endif // SPECMANAGECONNECTIONSCOMMAND_H
