#ifndef SPECMODEL_H
#define SPECMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QStringList>
#include "specstreamable.h"
#include "names.h"
#include "specfolderitem.h"

class specModel ;
class specMetaModel ;
class specModelItem ;
class specActionLibrary ;
class specMimeConverter ;
class specCanvasItem ;
class QValidator ;

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
protected:
	virtual QStringList dataTypes() const ;
public:
	specModel(QObject *par = 0) ;
	~specModel() ;
	QStringList mimeTypes() const ;
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
	void expandFolders(QModelIndexList&) const ;
	virtual bool insertItems(QList<specModelItem*> list, QModelIndex parent, int row=0) ;
	void fillSubMap(const QModelIndexList&) ;
	void applySubMap(const QModelIndexList&) ;
	const QStringList& descriptors() const ;
	const QList<spec::descriptorFlags> & descriptorProperties() const;
	void setDescriptorProperties(spec::descriptorFlags flags, const QString& key) ;
	void renameDescriptors(const QMap<QString, QString>&) ;
	void deleteDescriptor(const QString&) ;
	void dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const ;
	void restoreDescriptor(qint16 position, spec::descriptorFlags flags, QListIterator<specDescriptor>& origin, const QString& key) ;

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

	friend QDataStream& operator<<(QDataStream&, specModel&);
	friend QDataStream& operator>>(QDataStream&, specModel&);

	void signalBeginReset() { beginResetModel() ; } // TODO just emit from whereever this function is called
	void signalEndReset() { endResetModel() ; } // TODO just emit from whereever this function is called
	void signalChanged(const QModelIndex& index) ;
	void signalChanged(QModelIndex originalBegin, QModelIndex originalEnd) ;
	virtual QList<specFileImportFunction> acceptableImportFunctions() const ;

	virtual QValidator* createValidator(const QModelIndex&) const ;

	// //TODO
public slots:
	bool exportData(QModelIndexList& list) ;
	void svgMoved(specCanvasItem*, int, double, double) ;
};

#endif
