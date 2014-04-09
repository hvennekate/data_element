#ifndef SPECMULTIPLEITEMCOMMAND_H
#define SPECMULTIPLEITEMCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specMultipleItemCommand : public specUndoCommand
{
private:
	QVector<specGenealogy> items ;
	QMap<specGenealogy, QVector<QPair<int, int> > > clients ; // first: old index in client's server list, second: index in all items
	QMap<int, QVector<QPair<specGenealogy, bool> > > servers ; // bool: is meta?
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
	QList<specModelItem*> recursiveItemPointers() ;
	void addItems(QList<specModelItem*>&) ;
	void clearItems() ;
	QList<specModelItem*> firstItems() ;
public:
	specMultipleItemCommand(specUndoCommand* parent = 0) ;
	virtual void setItems(QList<specModelItem*>&) ;
};

#endif // SPECMULTIPLEITEMCOMMAND_H
