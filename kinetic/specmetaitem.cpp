#include "specmetaitem.h"

specMetaItem::specMetaItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  filter(0)
{
}

specPlot *specMetaItem::itemPlot()
{
	for (int i = 0 ; i < items.size() ; ++i)
		if (items[i]->plot())
			return items[i]->plot() ;
	return 0 ;
}

QDataStream &specMetaItem::writeToStream(QDataStream &out) const
{
	return out ;
}

QDataStream &specMetaItem::readFromStream(QDataStream &in)
{
	return in ;
}

QList<specGenealogy*> specMetaItem::purgeConnections()
{
	QList<specModelItem*> list = items ;
	while(!items.isEmpty())
		items.takeLast()->disconnectClient(this) ;
	invalidate() ;
	return list ;
}

bool specMetaItem::disconnectServer(specModelItem *server)
{
	if (!items.contains(server))
		return false ;
	items.removeOne(server) ;
	server->disconnectClient(this) ;
	invalidate() ;
	return true ;
}

bool specMetaItem::connectServer(specModelItem *server)
{
	if (items.contains(server))
		return false ;
	server->connectClient(this) ;
	items << server ;
	invalidate() ;
	return true ;
}

specMetaFilter *specMetaItem::takeFilter()
{
	specMetaFilter *oldFilter = filter ;
	filter = 0 ;
	invalidate() ;
	return oldFilter ;
}

void specMetaItem::setFilter(specMetaFilter *newFilter)
{
	filter = newFilter ;
	invalidate() ;
}

void specMetaItem::attach(QwtPlot *plot)
{
	specPlot *otherPlot = itemPlot() ;
	if (otherPlot && filter)
	{
		QList<specCanvasItem*> otherItems = filter->plotIndicators() ;
		foreach(specCanvasItem* item, otherItems)
			item->attach(otherPlot) ;
	}
	specModelItem::attach(plot) ;
}

void specMetaItem::revalidate()
{
	if (!filter)
	{
		setData(0); // TODO is this allowed?
		return ;
	}

	// TODO do some more checks on valid items etc.
	setData(filter->data(items)) ;
}

void specMetaItem::refreshPointers(const QHash<specModelItem *, specModelItem *> &mapping)
{// TODO this could be problematic with 32bit vs. 64bit systems...
	invalidate() ;
	QList<specModelItem*> newPointers ;
	foreach(specModelItem* pointer, items)
		newPointers << mapping[pointer] ;
	newPointers.removeAll(0) ;
	items = newPointers ;
}
