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
	QList<specModelItem*> Items ;
	bool owning ;
	bool knowingParent ;
	void getItemPointers() ;
public:
	explicit specGenealogy(QModelIndexList&);
	virtual ~specGenealogy() ;
	specGenealogy(specModel*, QDataStream&) ;
	void takeItems() ;
	void returnItems() ;
	bool valid() ;
	bool seekParent() ;
	specModel* model() ;
	specFolderItem* parent() ;
	const QList<specModelItem*>& items() const ;
	QDataStream& write(QDataStream&) ;
	QDataStream& read(specModel*, QDataStream&) ;
};

#endif // SPECGENEALOGY_H
