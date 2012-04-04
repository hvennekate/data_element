#ifndef SPECFOLDERITEM_H
#define SPECFOLDERITEM_H

#include "specmodelitem.h"
#include <QList>

/*! Class for folders in the specModel framework */
class specFolderItem : public specModelItem
{
private:
	QList<specModelItem*> childrenList ;
	bool suspendRefresh ;
protected:
	QDataStream& readFromStream(QDataStream&) ;
	QDataStream& writeToStream(QDataStream&) const ;
public:
	/*! Standard constructor (cf. specModelItem).*/
	specFolderItem(specFolderItem* par=0, QString description="");
	/*! Standard destructor.*/
	~specFolderItem();
	
	/*! Add an item as child at index \a position.*/
	bool addChild(specModelItem*, QList<specModelItem*>::size_type position) ;
	/*! Add all elements of \a list as children at index \a position.*/
	bool addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position = 0) ;
	/*! @todo remove children:  tell parent via destructor or let parent remove child? */
	void removeChild(specModelItem*) ;
	
	/*! Returns number of children*/
	QList<specModelItem*>::size_type children() const;
	/*! Wraps specModelItem if key is empty, otherwise returns true if \a key is editable in any child. */
	bool isEditable(QString key) const;
	/*! Wraps specModelItem's function if \a key is empty.  Otherwise \a key and \a value are passed on to children and the return value is the \a or result of the children's results. */
	bool changeDescriptor(QString key, QString value) ;
	/*! Refreshes Data used for plotting.  Depending on the values of \a mergePlotData and \a sortPlotData this may include merging and/or sorting of plot data obtained from children.*/
	void refreshPlotData() ;
	bool isFolder() const ;
	QStringList descriptorKeys() const ;
	QIcon decoration() const ;
	void buildTree(QStringList descriptors) ;
	specModelItem* child(QList<specModelItem*>::size_type i) const ;
	int childNo(specModelItem* child) const ;
	spec::descriptorFlags descriptorProperties(const QString& key) const ;
	/*! Reimplementation from specCanvasItem -- passing on to all children */
	void scaleBy(const double&) ;
	/*! Reimplementation from specCanvasItem -- passing on to all children */
	void addToSlope(const double&) ;
	/*! Reimplementation from specCanvasItem -- passing on to all children */
	void moveYBy(const double&) ;
	void moveXBy(const double &) ;
	void exportData(const QList<QPair<bool,QString> >&, const QList<QPair<spec::value,QString> >&, QTextStream&) ;
	void haltRefreshes(bool halt = true) ;
	void subMap(const QMap<double,double>&) ;
};

#endif
