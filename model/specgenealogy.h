#ifndef SPECGENEALOGY_H
#define SPECGENEALOGY_H

#include "specmodel.h"
#include <QVector>

class specGenealogy : public specStreamable
{
private:
	specModel* Model ;
	QVector<int> indexes ;
	specFolderItem* Parent ;
	QVector<specModelItem*> Items ;
	bool owning ;
	void getItemPointers() ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::genealogyId ; }
	specModelItem* factory(const type &t) const ;
public:
	explicit specGenealogy(QModelIndexList&);
	explicit specGenealogy(const QModelIndex&) ;
	virtual ~specGenealogy() ;
	specGenealogy(specModel*, QDataStream&) ;
	specGenealogy() ;
	void setModel(specModel* model) ;
	void takeItems() ;
	void returnItems() ;
	bool valid() ;
	bool seekParent() ;
	specModel* model() ;
	specFolderItem* parent() ;
	QVector<specModelItem*> items() ;
	specModelItem* firstItem() ;

	bool operator==(const specGenealogy& other) ;
	bool operator!=(const specGenealogy& other) ;
};

#endif // SPECGENEALOGY_H
