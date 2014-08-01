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

QDataStream& operator<< (QDataStream&, specModel&);
QDataStream& operator>> (QDataStream& in, specModel& model) ;

class specModel : public QAbstractItemModel, public specStreamable
{
	Q_OBJECT
private:
	specModelItem* root ;
	QStringList Descriptors ;
	QList<spec::descriptorFlags> DescriptorProperties ;
	QIcon descriptorIcon(int index) const ;
	void insertFromStream(QDataStream& stream, const QModelIndex& parent, int row) ;
	bool itemsAreEqual(QModelIndex& first, QModelIndex& second, const QList<QPair<QStringList::size_type, double> >& criteria) ;
	QModelIndexList merge(QModelIndexList& list, const QList<QPair<QStringList::size_type, double> >& criteria) ;
	QModelIndexList allChildren(const QModelIndex&) const ;
	QMap<double, double> subMap ;
	bool internalDrop ;
	specActionLibrary* dropBuddy ;
	int dontDelete ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::model ; }
	QList<specMimeConverter*> mimeConverters() const ;
	specMetaModel* metaModel ;
	void checkForNewDescriptors(const QList<specModelItem*>& list, const QModelIndex& parent) ;
	bool resetPending ;
	QSize generateSizeHint(const QModelIndex&) const ;
public:
	virtual QStringList dataTypes() const ;
	specModel(QObject* par = 0) ;
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
	static bool lessThanItemPointer(specModelItem*, specModelItem*) ;
	static bool lessThanHierarchies(const QVector<int>& a, const QVector<int>& b) ;
	static bool lessThanIndex(const QModelIndex&, const QModelIndex&) ;
	QList<specModelItem*> pointerList(const QModelIndexList&) const ;
	QModelIndexList indexList(const QList<specModelItem*>&) const ;
	bool contains(specModelItem*) const ;
	template<class T>
	QModelIndexList indexList(const QList<T*>& pointers) const
	{
		QModelIndexList returnList ;
		foreach(T * tpointer, pointers)
		{
			specModelItem* pointer = dynamic_cast<specModelItem*>(tpointer) ;
			if(pointer)
				returnList << index(pointer) ;
		}
		return returnList ;
	}

	bool isFolder(const QModelIndex&) const ;
	static void eliminateChildren(QModelIndexList&);
	static void eliminateChildren(QList<specModelItem*>&);
	static QList<specModelItem*> expandFolders(const QList<specModelItem*>&, bool retain = false) ;
	virtual bool insertItems(QList<specModelItem*> list, QModelIndex parent, int row = 0) ;
	void fillSubMap(const QModelIndexList&) ;
	void applySubMap(const QModelIndexList&) ;
	const QStringList& descriptors() const ;
	const QList<spec::descriptorFlags>& descriptorProperties() const;
	void setDescriptorProperties(spec::descriptorFlags flags, const QString& key) ;
	void renameDescriptors(const QMap<QString, QString>&) ;
	void deleteDescriptor(const QString&) ;
	void dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const ;
	void restoreDescriptor(qint16 position, spec::descriptorFlags flags, QListIterator<specDescriptor>& origin, const QString& key) ;

	// QAbstractItemModel virtual functions:

	int columnCount(const QModelIndex& parent) const ;
	int rowCount(const QModelIndex&) const ;
	bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex());

	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);
	bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());

	QModelIndex parent(const QModelIndex& index) const ;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const ;
	Qt::ItemFlags flags(const QModelIndex& index) const;

	QVariant data(const QModelIndex& index, int role) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;

	// 	// Drag and drop:
	void setInternalDrop(bool) ;
	void setDropBuddy(specActionLibrary*) ;
	Qt::DropActions supportedDropActions() const ;
	QMimeData* mimeData(const QModelIndexList&) const;
	bool dropMimeData(const QMimeData*, Qt::DropAction, int row, int column, const QModelIndex& parent) ;
	bool mimeAcceptable(const QMimeData*) const ;

	friend QDataStream& operator<< (QDataStream&, specModel&);
	friend QDataStream& operator>> (QDataStream&, specModel&);

	void signalBeginReset() ;
	void signalChanged(const QModelIndex& index) ;
	void signalChanged(QModelIndex originalBegin, QModelIndex originalEnd) ;
	virtual QList<specFileImportFunction> acceptableImportFunctions() const ;

	virtual QValidator* createValidator(const QModelIndex&) const ;

	// //TODO
private slots:
	void signalEndReset()  ;
public slots:
	void svgMoved(specCanvasItem*, int, double, double) ;
};

#endif
