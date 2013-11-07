#ifndef SPECGENEALOGY_H
#define SPECGENEALOGY_H

#include "specmodel.h"
#include <QVector>
#include "specmodelitem.h"

class specGenealogy : public specStreamable
{
	friend QTextStream& operator<< (QTextStream& out, const specGenealogy& g) ;
private:
	specModel* Model ;
	QVector<int> indexes ;
	specFolderItem* Parent ;
	QVector<specModelItem*> Items ;
	bool owning ;
	void getItemPointers() ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::genealogyId ; }
	specModelItem* factory(const type& t) const ;
	void clear() ;
public:
	specGenealogy(specModelItem* p, specModel* m) ;
	specGenealogy(QList<specModelItem*>& l, specModel* m) ;
	virtual ~specGenealogy() ;
	specGenealogy() ;
	void setModel(specModel* model) ;
	void takeItems() ;
	void returnItems() ;
	bool valid() const ;
	bool seekParent() ;
	int size() const { return Items.size() ; }
	void signalChange() const ;
	specModel* model() const ;
	specFolderItem* parent() const ;
	QVector<specModelItem*> items() ;
	QVector<specModelItem*> items() const ;
	specModelItem* firstItem() ;
	specModelItem* firstItem() const ;
	QModelIndex firstIndex() ;

	bool operator== (const specGenealogy& other) ;
	bool operator!= (const specGenealogy& other) ;
	specGenealogy& operator= (specGenealogy& other) ;
	specGenealogy& operator= (const specGenealogy& other) ;
};

QTextStream& operator<< (QTextStream& out, const specGenealogy& g) ;

#endif // SPECGENEALOGY_H
