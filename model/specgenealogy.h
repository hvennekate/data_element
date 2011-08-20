#ifndef SPECGENEALOGY_H
#define SPECGENEALOGY_H

#include "specmodel.h"
#include <QVector>

class specGenealogy
{
private:
	specModel* Model ;
	QVector<int> indexes ;
	specFolderItem* parent ;
	QList<specModelItem*> items ;
	bool owning ;
	void getItemPointers(const QList<specModelItem *> &) ;
public:
	specGenealogy(const QModelIndexList&);
	specGenealogy(const QList<specModelItem*>&, specModel*) ;
	specGenealogy(const QList<specModelItem*>&, int row, specFolderItem* parent, specModel*) ;
	void takeItems() ;
	void returnItems() ;
	bool valid() ;
	QDataStream& write(QDataStream&) ;
	QDataStream& read(QDataStream&) ;
};

#endif // SPECGENEALOGY_H
