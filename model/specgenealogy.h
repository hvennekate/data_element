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
	QDataStream& write(QDataStream&) const ;
	QDataStream& read(specModel*, QDataStream&) ;

	bool operator==(const specGenealogy& other) ;
	bool operator!=(const specGenealogy& other) ;
};

#endif // SPECGENEALOGY_H
