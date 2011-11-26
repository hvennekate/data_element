#ifndef SPECMETAITEM_H
#define SPECMETAITEM_H

#include "specmodelitem.h"
#include "specmetafilter.h"

class specMetaItem : public specModelItem
{
private:
	QList<specModelItem*> items ;
	specModel *model ;
	specMetaFilter *filter ;
	specPlot *itemPlot() ;
protected:
	QDataStream& readFromStream(QDataStream&) ;
	QDataStream& writeToStream(QDataStream &) const ;
	void revalidate() ;
public:
	explicit specMetaItem(specFolderItem* par=0, QString description="");
	void setModel(specModel*) ;
	QList<specModelItem*> purgeConnections() ;
	bool disconnectServer(specModelItem*) ;
	bool connectServer(specModelItem*) ;
	specMetaFilter *takeFilter() ;
	void setFilter(specMetaFilter*) ;
	void attach(QwtPlot *plot) ;
};

/* TODO in other classes

  specModelItem:
	disconnect from server if detachChildren() called
	install invalidate functionality
	install client functionality
	attach needs to revalidate data before connecting to plot

*/
#endif // SPECMETAITEM_H
