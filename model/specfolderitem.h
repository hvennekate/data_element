#ifndef SPECFOLDERITEM_H
#define SPECFOLDERITEM_H

#include "specmodelitem.h"
#include <QList>

/*! Class for folders in the specModel framework */
class specFolderItem : public specModelItem
{
private:
	QList<specModelItem*> ChildrenList ;
	void readFromStream(QDataStream& in) ;
	void writeToStream(QDataStream& out) const ;
	type typeId() const { return specStreamable::folder ; }
public:
	/*! Standard constructor (cf. specModelItem).*/
	specFolderItem(specFolderItem* par = 0, QString description = "");
	/*! Standard destructor.*/
	~specFolderItem();

	/*! find all descendants */
	template <typename T>
	QVector<T> findDescendants() ;
	/*! Add an item as child at index \a position.*/
	bool addChild(specModelItem*, QList<specModelItem*>::size_type position) ;
	/*! Add all elements of \a list as children at index \a position.*/
	bool addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position = 0) ;
	/*! @todo remove children:  tell parent via destructor or let parent remove child? */
	void removeChild(specModelItem*) ;
	/*! Returns number of children*/
	QList<specModelItem*>::size_type children() const;

	/*! Refreshes Data used for plotting.  Depending on the values of \a mergePlotData and \a sortPlotData this may include merging and/or sorting of plot data obtained from children.*/
	void refreshPlotData() ;
	bool isFolder() const ;

	QStringList descriptorKeys() const ;
	QIcon decoration() const ;
	specModelItem* child(QList<specModelItem*>::size_type i) const ;
	int childNo(specModelItem* child) const ;
	/*! Reimplementation from specCanvasItem -- passing on to all children */
	void addDataFilter(const specDataPointFilter&) ;
	QString exportData(const QList<QPair<bool, QString> > &headerFormat, const QList<QPair<int, QString> > &dataFormat, const QStringList &numericDescriptors) ;
	bool hasDescriptor(const QString &d) const ;
	void deleteDescriptor(const QString& key) ;
	void renameDescriptors(const QMap<QString, QString>& map);
	void dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const ;
	void restoreDescriptor(QListIterator<specDescriptor>& origin, const QString& key) ;
	QList<specModelItem*> childrenList() const ;
};

#endif
