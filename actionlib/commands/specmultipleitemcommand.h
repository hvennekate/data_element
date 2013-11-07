#ifndef SPECMULTIPLEITEMCOMMAND_H
#define SPECMULTIPLEITEMCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specMultipleItemCommand : public specUndoCommand
{
private:
	QVector<specGenealogy> items ;
	void writeCommand(QDataStream& out) const ;
	void readCommand(QDataStream& in);
protected:
	void writeItems(QDataStream&) const ;
	void readItems(QDataStream&, qint32 = -1) ;
	void takeItems() ;
	void restoreItems() ;
	void parentAssigned();
	qint32 itemCount() const ;
	QSet<specFolderItem*> parents() const;
	QList<specModelItem*> itemPointers() ;
	void addItems(QList<specModelItem*>&) ;
	void clearItems() ;
	QList<specModelItem*> firstItems() ;
public:
	specMultipleItemCommand(specUndoCommand* parent = 0) ;
	virtual void setItems(QList<specModelItem*>&) ;
};

#endif // SPECMULTIPLEITEMCOMMAND_H
