#ifndef SPECMETAITEM_H
#define SPECMETAITEM_H

#include "specmodelitem.h"
#include "specmetaparser.h"
#include "model/specdescriptor.h"
#include "specgenealogy.h"

class specPlot ;

class specMetaItem : public specModelItem
{
	friend class metaItemProperties ;
private:
	QList<specModelItem*> items ;
	specMetaParser *filter ;
	QHash<QString,specDescriptor> variables ;
	specModelItem *currentlyConnectingServer ;
	void refreshOtherPlots() ;
	type typeId() const { return specStreamable::metaItem ; }
	void readFromStream(QDataStream & in) ;
	void writeToStream(QDataStream & out) const ;
	specModel* metaModel, *dataModel ;
	QVector<QPair<specGenealogy,qint8> > oldConnections ;
public:
	void setModels(specModel* meta, specModel* data) ;
	bool disconnectServer(specModelItem*) ;
	bool connectServer(specModelItem*) ;
	explicit specMetaItem(specFolderItem* par=0, QString description="");
	QList<specModelItem*> purgeConnections() ;
	void attach(QwtPlot *plot) ;
	void refreshPointers(const QHash<specModelItem*,specModelItem*>& mapping) ; // TODO restore connections, maybe by hierarchy...
	void refreshPlotData();
	QStringList descriptorKeys() const ;
	QString descriptor(const QString &key, bool full=false) const ;
	bool changeDescriptor(QString key, QString value) ; // TODO add changeDescriptor(key,specDescriptor)
	spec::descriptorFlags descriptorProperties(const QString& key) const ;
	QIcon decoration() const ;
	void getRangePoint(int variable, int range, int point, double& x, double& y) const ;
	void setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY) ;
	bool setActiveLine(const QString &, int) ;
	int rtti() { return spec::metaItem ; }
	specUndoCommand* itemPropertiesAction(QObject *parentObject) ;
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
