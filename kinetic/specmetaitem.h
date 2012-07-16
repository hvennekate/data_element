#ifndef SPECMETAITEM_H
#define SPECMETAITEM_H

#include "specmodelitem.h"
#include "specmetaparser.h"
#include "model/specdescriptor.h"

class specPlot ;

class specMetaItem : public specModelItem
{
private:
	QSet<specModelItem*> items ;
	specMetaParser *filter ;
	QHash<QString,specDescriptor> variables ;
	specModelItem *currentlyConnectingServer ;
	void refreshOtherPlots() ;
	type id() const { return specStreamable::metaItem ; }
	void readFromStream(QDataStream & in) ;
	void writeToStream(QDataStream & out) const ;
public:

	bool disconnectServer(specModelItem*) ;
	bool connectServer(specModelItem*) ;
	explicit specMetaItem(specFolderItem* par=0, QString description="");
	QList<specModelItem*> purgeConnections() ;
	void attach(QwtPlot *plot) ;
	void detach();
	void refreshPointers(const QHash<specModelItem*,specModelItem*>& mapping) ;
	void refreshPlotData();
	QStringList descriptorKeys() const ;
	QString descriptor(const QString &key, bool full=false) const ;
	bool changeDescriptor(QString key, QString value) ; // TODO add changeDescriptor(key,specDescriptor)
	spec::descriptorFlags descriptorProperties(const QString& key) const ;
	QIcon decoration() const ;
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
