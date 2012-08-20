#include "specmetaitem.h"
#include "specplot.h"
#include "metaitemproperties.h"

specMetaItem::specMetaItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  filter(0),
	  currentlyConnectingServer(0),
	  metaModel(0),
	  dataModel(0)
{
	filter = new specMetaParser("","","",this) ;
	invalidate() ;
}

void specMetaItem::setModels(specModel *m, specModel *d)
{
	metaModel = m ;
	dataModel = d ;

	for (int i = 0 ; i < oldConnections.size() ; ++i)
	{
		oldConnections[i].first.setModel(oldConnections[i].second ? m : d) ;
		foreach(specModelItem* item, oldConnections[i].first.items())
			connectServer(item) ;
	}
	oldConnections.clear();
	invalidate();
}

void specMetaItem::writeToStream(QDataStream &out) const
{
	specModelItem::writeToStream(out) ;
	out << variables ;
	if (!metaModel || !dataModel) return ;
	QVector<QPair<specGenealogy,qint8> > currentConnections ;
	foreach(specModelItem* item, items)
	{
		QModelIndex index = metaModel->index(item) ;
		if (index.isValid())
			currentConnections << qMakePair<specGenealogy,qint8>(
						specGenealogy(QModelIndexList() << index), true) ;
		else currentConnections << qMakePair<specGenealogy,qint8>(
					specGenealogy(QModelIndexList() <<
						      dataModel->index(item)), false) ;
	}
	out << currentConnections ;
}

void specMetaItem::readFromStream(QDataStream &in)
{
	specModelItem::readFromStream(in) ;
	in >> variables >> oldConnections;
	invalidate() ; // TODO maybe insert in data item or just model item.
}

QList<specModelItem*> specMetaItem::purgeConnections()
{
	QList<specModelItem*> list = items ;
	foreach(specModelItem* item, items)
		item->disconnectClient(this) ;
	items.clear();
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
	currentlyConnectingServer = server ;
	if (!server->connectClient(this))
	{
		currentlyConnectingServer = 0 ;
		return false ;
	}
	currentlyConnectingServer = 0 ;
	items << server ;
	invalidate() ;
	return true ;
}

void specMetaItem::refreshOtherPlots()
{
	QSet<specPlot *> otherPlots ;
	foreach(specModelItem* item, items)
		otherPlots << ((specPlot*) item->plot()) ;
	otherPlots.remove(0) ;
	filter->attachRanges(otherPlots)  ;
	foreach(QwtPlot *otherPlot, otherPlots)
		otherPlot->replot();
}

void specMetaItem::attach(QwtPlot *plot)
{
	if (!plot)
	{
		filter->detachRanges();
		return ;
	}
	specModelItem::attach(plot) ;
	refreshOtherPlots() ;
}

void specMetaItem::refreshPlotData()
{
	// TODO do some more checks on valid items etc.
	foreach(specModelItem *item, items)
		item->revalidate();
	setData(processData(filter->evaluate(items.toVector()))) ; // TODO use vector
	variables["errors"] = filter->warnings() ;
	refreshOtherPlots() ;
}

QStringList specMetaItem::descriptorKeys() const
{
	QStringList keys = specModelItem::descriptorKeys() ;
	keys << "variables" << "x" << "y" << "errors" ;
	return keys ;
}

QString specMetaItem::descriptor(const QString &key, bool full) const
{
	if (key == "") return specModelItem::descriptor(key,full) ;
	return variables[key].content(full) ;
}

bool specMetaItem::changeDescriptor(QString key, QString value)
{
	if (key == "") return specModelItem::changeDescriptor(key,value) ;
	if (!descriptorKeys().contains(key)) return false ;
	variables[key] = value ;
	filter->setAssignments(variables["variables"].content(true), variables["x"].content(true), variables["y"].content(true)) ;
	invalidate();
	return true ;
}

spec::descriptorFlags specMetaItem::descriptorProperties(const QString &key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	return (key == "errors" ? spec::def : spec::editable) ;
}

QIcon specMetaItem::decoration() const
{
	if (!filter->ok())
		return QIcon::fromTheme("dialog-warning") ;

	return QIcon(":/kinetic.png") ;
}

void specMetaItem::getRangePoint(int variable, int range, int point, double &x, double &y) const
{
	filter->getRangePoint(variable, range,point,x,y) ;
}

void specMetaItem::setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY)
{
	filter->setRange(variableNo, rangeNo, pointNo, newX, newY) ;
}

bool specMetaItem::setActiveLine(const QString &s, int i)
{
	if (variables.contains(s))
	{
		variables[s].setActiveLine(i) ;
		return true ;
	}
	return specModelItem::setActiveLine(s,i) ;
}

specUndoCommand* specMetaItem::itemPropertiesAction(QObject *parentObject)
{
	metaItemProperties propertiesDialog(this) ;
	propertiesDialog.exec() ;
	if (propertiesDialog.result() != QDialog::Accepted) return 0 ;
	return propertiesDialog.changedConnections(parentObject) ;
}
