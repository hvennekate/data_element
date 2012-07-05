#ifndef SPECMODELITEM_H
#define SPECMODELITEM_H

#include <QString>
#include <QMultiMap>
#include "plot/speccanvasitem.h"
#include <QIcon>
#include <QStringList>
#include "names.h"
#include <QDataStream>
#include <QTextStream>
#include "specrange.h"
#include "specdescriptor.h"

class specModelItem ;
class specFolderItem ;
class specKineticRange ;
class specMetaItem ;

QDataStream& operator>>(QDataStream&, specModelItem*&) ;
// QDataStream& operator<<(QDataStream&, specModelItem*&) ;

/*! Base class of list items. */
class specModelItem : public specCanvasItem
{
private:
	specFolderItem* iparent ;
	void detachChild(specModelItem* child) ;
	bool dataValid ;
	QSet<specMetaItem*> clients ;
	specDescriptor description ;
protected:
	void processData(QVector<double>&, QVector<double>&) const ;
	QwtSeriesData<QPointF>* processData(QwtSeriesData<QPointF>* dat) const;
	// selectedPoints (3 Punkte fuer Korrekturen)
	virtual bool shortCircuit(specModelItem* server) ;
	virtual QDataStream& readFromStream(QDataStream&) =0 ;
	virtual QDataStream& writeToStream(QDataStream&) const =0 ;
public:
	void revalidate() ;
	void invalidate() ;
	virtual bool connectServer(specModelItem*) ;
	virtual bool disconnectServer(specModelItem*) ;
	bool connectClient(specMetaItem* clnt) ;
	bool disconnectClient(specMetaItem* clnt) ;
	/*! Merge/sort plot data when rebuilding it.  Mainly applies to both specFolderItem and specDataItem.  Both are enabled by default. */
	bool mergePlotData, sortPlotData ;
	
	/*! Standard constructor.  Expects pointer to parent (zero for root item) and possibly an initial value for the descriptive tag (default: empty string). */
	specModelItem(specFolderItem* par=0, QString description="");
	/*! Standard destructor.*/
	virtual ~specModelItem();
	/*! Returns a collection of data points (normally empty, needs to be reimplemented in subclasses). The format is time, signal.*/
	virtual QMultiMap<double,QPair<double,double> >* kinetics(QList<specKineticRange*>) const;
	
	/*! Assign parent. */
	void setParent(specFolderItem*) ; // TODO make private and friend of folder class
	/*! Returns pointer to currently assigned parent. */
	specFolderItem* parent() const;
	/*! Returns number of children.  Normally none, so this function needs to be reimplemented if the item is to be a folder or similar type. */
	virtual QList<specModelItem*>::size_type children() const;
	/*! Is this item a folder? */
	virtual bool isFolder() const ;
	/*! Return value of descriptor \a key */
	virtual QString descriptor(const QString &key, bool full=false) const ;
	/*! Is the descriptor \a key editable?*/
	virtual bool isEditable(QString key) const ;
	/*! Set descriptor \a key 's value to  \a value. */
	virtual bool changeDescriptor(QString key, QString value) ; // TODO add changeDescriptor(key,specDescriptor)
	virtual bool setActiveLine(const QString&, int) ;
	virtual int activeLine(const QString& key) const ;
	/*! Regenerate data used for plotting. */
	virtual void refreshPlotData() ; // TODO make private/protected
	/*! Try to set data whose \f$\nu\f$ value is in \a ranges to zero by applying some correction (preferrably by subtracting a linear function previously fit to the data concerned).*/
	virtual QIcon decoration() const ;
	QIcon indicator(const QString&) const ;
	virtual bool addChild(specModelItem *child, QList<specModelItem*>::size_type position) ;
	virtual bool addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position) ;
	virtual QStringList descriptorKeys() const ;
	virtual spec::descriptorFlags descriptorProperties(const QString& key) const ;
	virtual void exportData(const QList<QPair<bool,QString> >&, const QList<QPair<spec::value,QString> >&, QTextStream&) ;
	virtual QVector<double> intensityData() const ;
	virtual int removeData(QList<specRange*>*) { refreshPlotData() ; return 0 ; }
	virtual void movingAverage(int) {}
	virtual void average(int) {}
	virtual void subMap(const QMap<double,double>&) {}

	int rtti() const { return spec::spectrum ; }
	
	friend QDataStream& operator>>(QDataStream&, specModelItem*&) ;
	QDataStream& writeOut(QDataStream&) const;
};

#endif
