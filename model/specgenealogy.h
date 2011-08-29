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
	specGenealogy(specModel*, QDataStream&) ;
	void takeItems() ;
	void returnItems() ;
	bool valid() ;
	bool seekParent() ;
	specModel* model() ;
	specFolderItem* parent() ;
	QDataStream& write(QDataStream&) ;
	QDataStream& read(specModel*, QDataStream&) ;
};

#endif // SPECGENEALOGY_H
