#ifndef SPECGENEALOGY_H
#define SPECGENEALOGY_H

#include "specmodel.h"
#include <QVector>

class specGenealogy
{
private:
	specModel* Model ;
	QVector<int> indexes ;
	specFolderItem* Parent ;
	QList<specModelItem*> items ;
	bool owning ;
	void getItemPointers(const QList<specModelItem *> &) ;
public:
	explicit specGenealogy(QModelIndexList&);
	void takeItems() ;
	void returnItems() ;
	bool valid() ;
	specModel* model() ;
	specFolderItem* parent() ;
	QDataStream& write(QDataStream&) ;
	QDataStream& read(QDataStream&) ;
};

#endif // SPECGENEALOGY_H
