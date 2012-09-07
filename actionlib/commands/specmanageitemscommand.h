#ifndef SPECMANAGEITEMSCOMMAND_H
#define SPECMANAGEITEMSCOMMAND_H

#include "specmodel.h"
#include "specundocommand.h"
#include <QVector>
#include <QPair>
#include <QHash>
#include "specgenealogy.h"

class specManageItemsCommand : public specUndoCommand
{
public:
	explicit specManageItemsCommand(specUndoCommand *parent = 0);
	virtual ~specManageItemsCommand() ;
	bool ok() ;
	void setItems(QModelIndexList&) ;
protected:
	void take() ;
	void restore() ;
private:
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	QVector<specGenealogy*> items ;
	bool inverse() ;
	void clear() ;
	void prepare() ;
	void finish() ;
	void parentAssigned();
};

#endif // SPECDELETECOMMAND_H
