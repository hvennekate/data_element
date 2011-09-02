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

class specModel ;

QDataStream& operator<<(QDataStream&, specModel&);
QDataStream& operator>>(QDataStream& in, specModel& model) ;

// TODO put these funktions into a sensible context
bool lessThanIndex(const QModelIndex&, const QModelIndex&) ;
int indexLevel(const QModelIndex& index) ;

class specModel : public QAbstractItemModel
{
	Q_OBJECT
private:
	specModelItem* root ;
	QStringList descriptors ;
	QList<spec::descriptorFlags> descriptorProperties ;
	void insertFromStream(QDataStream& stream, const QModelIndex& parent, int row) ;
	bool getMergeCriteria(QList<QPair<QStringList::size_type, double> >&) ;
	bool itemsAreEqual(QModelIndex& first, QModelIndex& second, const QList<QPair<QStringList::size_type, double> >& criteria) ;
	QModelIndexList merge(QModelIndexList& list, const QList<QPair<QStringList::size_type, double> >& criteria) ;
	QModelIndexList allChildren(const QModelIndex&) const ;
	QStringList mime ;
	QMap<double,double> subMap ;
public:
	specModel(QObject *par = 0) ;
	specModel(QDataStream&, QObject *par = 0) ;
	~specModel() ;
	
	// Own functions
	specModelItem* itemPointer(const QModelIndex&) const ;
	specModelItem* itemPointer(const QVector<int>&) const ;
	static QVector<int> hierarchy(specModelItem*) ;
	static QVector<int> hierarchy(const QModelIndex&) ;
	bool isFolder(const QModelIndex&) const ;
	void eliminateChildren(QModelIndexList&) const ;
	virtual bool insertItems(QList<specModelItem*> list, QModelIndex parent, int row=0) ;
	void fillSubMap(const QModelIndexList&) ;
	void applySubMap(const QModelIndexList&) ;
	
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
	Qt::DropActions supportedDropActions() const ;
	QStringList mimeTypes() const ;
	void setMimeTypes(const QStringList&) ;
	QMimeData *mimeData(const QModelIndexList &) const;
	bool dropMimeData(const QMimeData*, Qt::DropAction, int row, int column, const QModelIndex &parent) ;
	
	// Comfort
	bool buildTree(const QModelIndex&) ;
	QModelIndexList mergeItems(QModelIndexList&) ;
	
	friend QDataStream& operator<<(QDataStream&, specModel&);
	friend QDataStream& operator>>(QDataStream&, specModel&);

	void signalBeginReset() { beginResetModel() ; }
	void signalEndReset() { endResetModel() ; }
	
	
// //TODO	bool QAbstractItemModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole )
// //TODO	
public slots:
	void importFile(QModelIndex) ;
	bool exportData(QModelIndexList& list) ;
// 	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
};

#endif
