#ifndef SPECMODEL_H
#define SPECMODEL_H

#include <QAbstractItemModel>
#include "specfolderitem.h"
#include "specdataitem.h"
#include "specmodelitem.h"
#include <QList>
#include <QObject>
#include <QDataStream>
#include <QMimeData>
#include <QDataStream>
#include <names.h>
#include <QPair>
#include <QItemSelection>
#include <QMap>
#include <QHash>
#include "specactionlibrary.h"
#include "specmimeconverter.h"

class specModel ;
class specMetaModel ;

QDataStream& operator<<(QDataStream&, specModel&);
QDataStream& operator>>(QDataStream& in, specModel& model) ;

// TODO put these funktions into a sensible context
bool lessThanIndex(const QModelIndex&, const QModelIndex&) ;
int indexLevel(const QModelIndex& index) ;

class specView ;

class specModel : public QAbstractItemModel, public specStreamable
{
	Q_OBJECT
private:
	specModelItem* root ;
	QStringList Descriptors ;
	QList<spec::descriptorFlags> DescriptorProperties ;
	void insertFromStream(QDataStream& stream, const QModelIndex& parent, int row) ;
	bool getMergeCriteria(QList<QPair<QStringList::size_type, double> >&) ;
	bool itemsAreEqual(QModelIndex& first, QModelIndex& second, const QList<QPair<QStringList::size_type, double> >& criteria) ;
	QModelIndexList merge(QModelIndexList& list, const QList<QPair<QStringList::size_type, double> >& criteria) ;
	QModelIndexList allChildren(const QModelIndex&) const ;
	QMap<double,double> subMap ;
	bool internalDrop ;
	specView* dropSource ;
	specActionLibrary* dropBuddy ;
	int dontDelete ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::model ; }
	QList<specMimeConverter*> mimeConverters() const ;
	specMetaModel* metaModel ;
	void checkForNewDescriptors(const QList<specModelItem*>& list, const QModelIndex& parent) ;
public:
	specModel(QObject *par = 0) ;
	~specModel() ;
	void setMetaModel(specMetaModel*) ;
	specMetaModel* getMetaModel() const ;
	// Own functions
	specModelItem* itemPointer(const QModelIndex&) const;
	specModelItem* itemPointer(const QVector<int>&) const;
	QModelIndex index(const QVector<int>&, int column = 0) const ;
	QModelIndex index(specModelItem*, int column = 0) const ;
	static QVector<int> hierarchy(specModelItem*) ;
	static QVector<int> hierarchy(const QModelIndex&) ;
	QList<specModelItem*> pointerList(const QModelIndexList&) const ;
	QModelIndexList indexList(const QList<specModelItem*>&) const ;
	bool contains(specModelItem*) const ;
	template<class T>
	QModelIndexList indexList(const QList<T*>& pointers) const
	{
		QModelIndexList returnList ;
		foreach(T* tpointer, pointers)
		{
			specModelItem *pointer = dynamic_cast<specModelItem*>(tpointer) ;
			if (pointer)
				returnList << index(pointer) ;
		}
		return returnList ;
	}

	bool isFolder(const QModelIndex&) const ;
	void eliminateChildren(QModelIndexList&) const ;
	virtual bool insertItems(QList<specModelItem*> list, QModelIndex parent, int row=0) ;
	void fillSubMap(const QModelIndexList&) ;
	void applySubMap(const QModelIndexList&) ;
	const QStringList& descriptors() const ;
	const QList<spec::descriptorFlags> & descriptorProperties() const;
	
	// QAbstractItemModel virtual functions:
	
	int columnCount(const QModelIndex &parent) const ;
	int rowCount(const QModelIndex&) const ;
	bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());
	
	bool setHeaderData (int section,Qt::Orientation orientation,const QVariant & value,int role = Qt::EditRole);
	bool insertColumns (int column,int count,const QModelIndex & parent = QModelIndex() );
	bool removeColumns (int column,int count,const QModelIndex & parent = QModelIndex() );
	
	QModelIndex parent(const QModelIndex & index) const ;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex() ) const ;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
	
// 	// Drag and drop:
	void setInternalDrop(bool) ;
	void setDropSource(specView*) ;
	void setDropBuddy(specActionLibrary*) ;
	Qt::DropActions supportedDropActions() const ;
	QMimeData *mimeData(const QModelIndexList &) const;
	bool dropMimeData(const QMimeData*, Qt::DropAction, int row, int column, const QModelIndex &parent) ;
	bool mimeAcceptable(const QMimeData*) const ;
	
	// Comfort
	bool buildTree(const QModelIndex&) ;
	QModelIndexList mergeItems(QModelIndexList&) ;
	
	friend QDataStream& operator<<(QDataStream&, specModel&);
	friend QDataStream& operator>>(QDataStream&, specModel&);

	void signalBeginReset() { beginResetModel() ; } // TODO just emit from whereever this function is called
	void signalEndReset() { endResetModel() ; } // TODO just emit from whereever this function is called
	void signalChanged(const QModelIndex& index) ;
	
	
// //TODO	bool QAbstractItemModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole )
// //TODO	
public slots:
	void importFile(QModelIndex) ;
	bool exportData(QModelIndexList& list) ;
	void svgMoved(specCanvasItem*, int, double, double) ;
// 	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
};

#endif
