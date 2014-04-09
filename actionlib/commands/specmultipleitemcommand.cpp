#include "specmultipleitemcommand.h"
#include "specmetaitem.h"
#include "specmetamodel.h"

specMultipleItemCommand::specMultipleItemCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}

void specMultipleItemCommand::clearItems()
{
	items.clear();
	clients.clear();
	servers.clear();
}

void specMultipleItemCommand::addItems(QList<specModelItem*>& list)
{
	// assumes everything has been cleared before!
	specModel* m = model() ;
	if(!m) return ;
	// get items
	while(!list.isEmpty())
		items << specGenealogy(list, m) ;

	specMetaModel* metaM = m->getMetaModel() ;
	if (!metaM) return ;
	specModel* dataM = metaM->getDataModel() ;
	if (!dataM) return ;
	QList<specModelItem*> allItems = recursiveItemPointers() ;
	for (int i = 0 ; i < allItems.size() ; ++i)
	{
		// get clients
		QSet<specMetaItem*> itemClients = allItems[i]->clientList() ;
		foreach(specMetaItem* client, itemClients)
			clients[specGenealogy(client, metaM)]
					<< qMakePair(client->serverIndex(allItems[i]), i) ;
		// get servers
		specMetaItem* mItem = dynamic_cast<specMetaItem*>(allItems[i]) ;
		if (!mItem) continue ;
		QList<specModelItem*> serverList = mItem->serverList() ;
		while(!serverList.empty())
		{
			if (dataM->contains(serverList.first()))
				servers[i] << qMakePair(specGenealogy(serverList, dataM), false) ;
			else if (metaM->contains(serverList.first()))
				servers[i] << qMakePair(specGenealogy(serverList, metaM), true) ;
			else
				serverList.takeFirst()->disconnectClient((specMetaItem*) (allItems[i])) ;
		}
	}


	foreach(const specGenealogy& gen, clients.keys())
		qSort(clients[gen]) ;
}

void specMultipleItemCommand::setItems(QList<specModelItem*>& list)
{
	clearItems();
	addItems(list) ;
}

QList<specModelItem*> specMultipleItemCommand::firstItems()
{
	QList<specModelItem*> l ;
	for(int i = 0 ; i < items.size() ; ++i)
		l << items[i].firstItem() ;
	return l ;
}

QList<specModelItem*> specMultipleItemCommand::itemPointers()
{
	QList<specModelItem*> l ;
	for(int i = 0 ; i < items.size() ; ++i)
		l << items[i].items().toList() ; // TODO avoid!
	return l ;
}

QList<specModelItem*> specMultipleItemCommand::recursiveItemPointers()
{
	return specModel::expandFolders(itemPointers(), true) ;
}

qint32 specMultipleItemCommand::itemCount() const
{
	return items.size() ;
}

void specMultipleItemCommand::writeCommand(QDataStream& out) const
{
	out << itemCount() ;
	writeItems(out);
	out << clients
	    << servers ;
}

void specMultipleItemCommand::readCommand(QDataStream& in)
{
	qint32 num = 0;
	in >> num;
	readItems(in, num);
	clients.clear();
	servers.clear();
	if (in.atEnd()) return ;
	in >> clients
	   >> servers ;
}

void specMultipleItemCommand::writeItems(QDataStream& out) const
{
	for(int i = 0 ; i < items.size() ; ++i)
		out << items[i] ;
}

void specMultipleItemCommand::takeItems()
{
	specModel* m = model() ;
	if(!m) return ;
	m->signalBeginReset();
	for(int i = 0 ; i < items.size() ; ++i)
		items[i].takeItems();
	foreach(specModelItem* item, recursiveItemPointers())
	{
		foreach(specMetaItem *client, item->clientList())
			item->disconnectClient(client) ;
		specMetaItem* metaItem = dynamic_cast<specMetaItem*>(item) ;
		if (!metaItem) continue ;
		foreach(specModelItem* server, metaItem->serverList())
			metaItem->disconnectServer(server) ;
	}
}

void specMultipleItemCommand::restoreItems()
{
	specModel* m = model() ;
	if(!m) return ;
	m->signalBeginReset();
	for(int i = 0 ; i < items.size() ; i++)
		items[i].returnItems();

	specMetaModel* metaM = m->getMetaModel() ;
	if (!metaM) return ;
	QList<specModelItem*> allItems = recursiveItemPointers() ;
	// reconnect to clients

	foreach(const specGenealogy& gen, clients.keys())
	{
		specGenealogy myGen = gen;
		myGen.setModel(metaM) ;
		specMetaItem* metaItem = dynamic_cast<specMetaItem*>(myGen.firstItem()) ;
		if (!metaItem) continue ;
		typedef QPair<int,int> intintPair ;
		foreach(const intintPair& server, clients[gen])
			metaItem->connectServer(allItems[server.second], server.first) ;
	}

	specModel* dataM = metaM->getDataModel() ;
	if (!dataM) return ;
	// reconnect to servers
	foreach(const int &itemIndex, servers.keys())
	{
		specMetaItem *metaItem = dynamic_cast<specMetaItem*>(allItems[itemIndex]) ;
		if (!metaItem) continue ;
		typedef QPair<specGenealogy, bool> genBoolPair ;
		foreach(genBoolPair clientGen, servers[itemIndex])
		{
			clientGen.first.setModel(clientGen.second ?
							 metaM :
							 dataM);
			foreach(specModelItem* item, clientGen.first.items())
				metaItem->connectServer(item) ;
		}
	}
}

void specMultipleItemCommand::readItems(QDataStream& in, qint32 n)
{
	items.clear();
	items.resize(n);
	for(int i = 0 ; i < n ; ++i)
		in >> items[i] ;
}

void specMultipleItemCommand::parentAssigned()
{
	specModel* model = dynamic_cast<specModel*>(parentObject()) ;
	Q_ASSERT(model) ;
	for(int i = 0 ; i < items.size() ; ++i)
		items[i].setModel(model) ;
}

QSet<specFolderItem*> specMultipleItemCommand::parents() const
{
	QSet<specFolderItem*> ps ;
	for(int i = 0 ; i < items.size() ; ++i)
		ps << items[i].parent() ;
	ps.remove(0) ;
	return ps ;
}
