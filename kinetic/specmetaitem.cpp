#include "specmetaitem.h"
#include "specplot.h"

specMetaItem::specMetaItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  filter("","",""),
	  currentlyConnectingServer(0)
{
}

specPlot *specMetaItem::itemPlot()
{
	for (int i = 0 ; i < items.size() ; ++i)
		if (items[i]->plot())
			return ((specPlot*) (items[i]->plot())) ;
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

QList<specModelItem*> specMetaItem::purgeConnections()
{
	QList<specModelItem*> list = items ;
	while(!items.isEmpty())
		items.takeLast()->disconnectClient(this) ;
	invalidate() ;
	return list ;
}

bool specMetaItem::disconnectServer(specModelItem *server) // TODO template ? -> short circ?
{
	if (server == currentlyConnectingServer)
		return true ;
	if (!items.contains(server))
		return false ;
	currentlyConnectingServer = server ;
	if (!server->disconnectClient(this))
	{
		currentlyConnectingServer = 0 ;
		return false ;
	}
	currentlyConnectingServer = 0 ;
	items.removeOne(server) ;
	invalidate() ;
	return true ;
}

bool specMetaItem::connectServer(specModelItem *server)
{
	if (currentlyConnectingServer == server)
		return true ;
	if (items.contains(server))
		return false ;
	if (shortCircuit(server))
		return false ;
	qDebug() << "connecting server" << server << this;
	currentlyConnectingServer = server ;
	if (!server->connectClient(this))
	{
		currentlyConnectingServer = 0 ;
		return false ;
	}
	currentlyConnectingServer = 0 ;
	items << server ;
	invalidate() ;
	qDebug() << "done connecting server" << this ;
	return true ;
}

//specMetaParser *specMetaItem::takeFilter()
//{
//	specMetaParser *oldFilter = filter ;
//	filter = 0 ;
//	invalidate() ;
//	return oldFilter ;
//}

//void specMetaItem::setFilter(specMetaFilter *newFilter)
//{
//	filter = newFilter ;
//	invalidate() ;
//}

void specMetaItem::attach(QwtPlot *plot)
{
	specPlot *otherPlot = itemPlot() ;
//	if (otherPlot && filter)
//	{
//		QList<specCanvasItem*> otherItems = filter->plotIndicators() ;
//		foreach(specCanvasItem* item, otherItems)
//			item->attach(otherPlot) ;
//	}
	specModelItem::attach(plot) ;
}

void specMetaItem::revalidate()
{
	qDebug() << "revalidating meta item" ;
	refreshPlotData();
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

void specMetaItem::refreshPlotData()
{
//	if (!filter)
//	{
//		setData(0); // TODO is this allowed?
//		return ;
//	}
	// TODO do some more checks on valid items etc.
	foreach(specModelItem *item, items)
		item->revalidate();
	filter.setAssignments(variables["variables"].content(true), variables["x"].content(true), variables["y"].content(true)) ;
	setData(filter.evaluate(items.toVector())) ; // TODO use vector
	variables["errors"] = filter.warnings() ;
	qDebug() << "Filter data size:" << dataSize() ;
	for (int i = 0 ; i < dataSize() ; ++i)
		qDebug() << "Filter data:" << sample(i).toPoint() ;
}

//QStringList specMetaItem::internalDescriptors() const
//{
//	if (!filter) return QStringList() ;
//	return filter->variables() ;
//}

//int specMetaItem::descriptorIndex(const QString& Descriptor) const
//{
//	return internalDescriptors().indexOf(Descriptor) ;
//}

QStringList specMetaItem::descriptorKeys() const
{
	QStringList keys = specModelItem::descriptorKeys() ;
//	keys << internalDescriptors() ;
	keys << "variables" << "x" << "y" << "errors" ;
	return keys ;
}

QString specMetaItem::descriptor(const QString &key, bool full) const
{
	if (key == "") return specModelItem::descriptor(key,full) ;
	return variables[key].content(full) ;
//	if (key == "variables")
//	int pos = descriptorIndex(key) ;
//	if (pos < 0) return QString() ;
//	qDebug() << "descriptor sizes:" << descriptorKeys() << filter->variableValues() ;
//	return filter->variableValues().at(pos) ;
}

bool specMetaItem::changeDescriptor(QString key, QString value)
{
	if (key == "") return specModelItem::changeDescriptor(key,value) ;
	if (!descriptorKeys().contains(key)) return false ;
	variables[key] = value ;
	refreshPlotData(); // TODO actually just invalidate
	return true ;
//	int pos = descriptorIndex(key) ;
//	if (pos < 0) return false ;
//	filter->setVariableValue(value,pos) ;
//	return true ;
}

spec::descriptorFlags specMetaItem::descriptorProperties(const QString &key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	return (key == "errors" ? spec::def : spec::editable) ;
}

