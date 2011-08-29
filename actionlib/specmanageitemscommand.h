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
	bool ok() ;
	void setItems(QModelIndexList&) ;
	QDataStream& write(QDataStream &) ;
	QDataStream& read(QDataStream &) ;
protected:
	void take() ;
	void restore() ;
private:
	QVector<specGenealogy*> items ;
	bool inverse() ;
	void clear() ;
	void prepare() ;
	void finish() ;
};

#endif // SPECDELETECOMMAND_H
