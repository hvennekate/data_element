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
class specMetaItem ;
class specUndoCommand ;

/*! Base class of list items. */
class specModelItem : public specCanvasItem
{
	friend class tst_movePlotCommand ;
private:
	specFolderItem* iparent ;
	void detachChild(specModelItem* child) ;
	bool dataValid ;
	QSet<specMetaItem*> clients ;
	specDescriptor description ;
	void processData() ;
	void initializeData();
protected:
	virtual void refreshPlotData() ; // TODO make private/protected
	// selectedPoints (3 Punkte fuer Korrekturen)
	virtual bool shortCircuit(specModelItem* server) ;
	virtual QString exportX(int index) const ;
	virtual QString exportY(int) const ;
	virtual QString exportZ(int index) const ;
	virtual QString exportCoreData(const QList<QPair<int, QString> > &dataFormat, const QStringList &numericDescriptors) const ;
	void readFromStream(QDataStream&) ;
	void writeToStream(QDataStream&) const ;
	specModelItem* factory(const type&) const ;
public:
	// Data
	void revalidate() ;
	void invalidate() ;
	virtual bool connectServer(specModelItem*) ;
	virtual bool disconnectServer(specModelItem*) ;
	bool connectClient(specMetaItem* clnt) ;
	bool disconnectClient(specMetaItem* clnt) ;
	virtual QList<specModelItem*> serverList() const { return QList<specModelItem*>() ; }
	QSet<specMetaItem*> clientList() const { return clients ; }

	/*! Merge/sort plot data when rebuilding it.  Mainly applies to both specFolderItem and specDataItem.  Both are enabled by default. */
	bool mergePlotData, sortPlotData ; // TODO loswerden

	/*! Standard constructor.  Expects pointer to parent (zero for root item) and possibly an initial value for the descriptive tag (default: empty string). */
	specModelItem(specFolderItem* par = 0, QString description = "");
	specModelItem(const specModelItem&) ;
	/*! Standard destructor.*/
	virtual ~specModelItem();

	/*! Assign parent. */
	void setParent(specFolderItem*) ;  // TODO make private and friend of folder class
	/*! Returns pointer to currently assigned parent. */
	specFolderItem* parent() const;
	/*! Returns number of children.  Normally none, so this function needs to be reimplemented if the item is to be a folder or similar type. */
	virtual QList<specModelItem*>::size_type children() const;
	virtual bool addChild(specModelItem* child, QList<specModelItem*>::size_type position) ;
	virtual bool addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position) ;
	/*! Is this item a folder? */
	virtual bool isFolder() const ;

	//Descriptors
	virtual specDescriptor getDescriptor(const QString& key) const ;
	virtual QString editDescriptor(const QString& key) const ;
	QString descriptor(const QString& key, bool full = false) const ;
	double descriptorValue(const QString& key) const ;
	quint32 activeLine(const QString& key) const ;
	bool isMultiline(const QString& key) const ;
	virtual QString toolTip(const QString& column) const ;

	virtual void changeDescriptor(const QString& key, QString value) ;  // TODO add changeDescriptor(key,specDescriptor)
	virtual void setActiveLine(const QString&, quint32) ;
	virtual void setMultiline(const QString& key, bool on = true) ;

	virtual QStringList descriptorKeys() const ; // TODO change to QSet
	virtual bool hasDescriptor(const QString &d) const ;
	virtual void renameDescriptors(const QMap<QString, QString>& map) ;
	virtual void deleteDescriptor(const QString& key) ;
	virtual void dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const ;
	virtual void restoreDescriptor(QListIterator<specDescriptor>& origin, const QString& key) ;

	/*! Try to set data whose \f$\nu\f$ value is in \a ranges to zero by applying some correction (preferrably by subtracting a linear function previously fit to the data concerned).*/
	virtual QIcon decoration() const ;
	QIcon indicator(const QString&) const ;

	virtual QString exportData(const QList<QPair<bool, QString> >&, const QList<QPair<int, QString> > &, const QStringList &numericDescriptors) ;
	virtual QVector<double> intensityData() const ;
	virtual void movingAverage(int) {}
	virtual void average(int) {}

	int rtti() const { return spec::spectrum ; } // TODO zu dataItem verschieben.
	void attach(QwtPlot* plot) ;
	void detach();

	static specModelItem* itemFactory(specStreamable::type) ;
	virtual specUndoCommand* itemPropertiesAction(QObject* parentObject) { Q_UNUSED(parentObject) ; return 0 ; }
	template <typename T>
	QVector<T> findDescendants() ;

	class descriptorComparison
	{
	private:
		const QStringList* description ;
	public:
		descriptorComparison(const QStringList* description) ;
		bool operator()(specModelItem*&, specModelItem*&) ;
	};
};

#endif
