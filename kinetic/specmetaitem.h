#ifndef SPECMETAITEM_H
#define SPECMETAITEM_H

#include "specmodelitem.h"
#include "specmetafilter.h"

class specPlot ;
class specMetaFilter ;

class specMetaItem : public specModelItem
{
private:
	QList<specModelItem*> items ;
	specMetaFilter *filter ;
	specPlot *itemPlot() ;
	QStringList internalDescriptors() const ;
	int descriptorIndex(const QString&) const ;
	specModelItem *currentlyConnectingServer ;
protected:
	QDataStream& readFromStream(QDataStream&) ;
	QDataStream& writeToStream(QDataStream &) const ;
public:
	bool disconnectServer(specModelItem*) ;
	bool connectServer(specModelItem*) ;
	void revalidate() ;
	explicit specMetaItem(specFolderItem* par=0, QString description="");
	QList<specModelItem*> purgeConnections() ;
	specMetaFilter *takeFilter() ;
	void setFilter(specMetaFilter*) ;
	void attach(QwtPlot *plot) ;
	void refreshPointers(const QHash<specModelItem*,specModelItem*>& mapping) ;
	void refreshPlotData();
	QStringList descriptorKeys() const ;
	QString descriptor(const QString &key, bool full=false) const ;
	bool changeDescriptor(QString key, QString value) ; // TODO add changeDescriptor(key,specDescriptor)
};

/* TODO in other classes

  specModelItem:
	disconnect from server if detachChildren() called
	.install invalidate functionality
	.install client functionality
	attach needs to revalidate data before connecting to plot
	make use of invalidate in refreshPlotData etc.
	make use of client in destructor etc.

*/
#endif // SPECMETAITEM_H
