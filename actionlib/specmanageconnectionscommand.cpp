#include "specmanageconnectionscommand.h"
#include "kinetic/specmetaitem.h"
#include "specplotwidget.h"

specManageConnectionsCommand::specManageConnectionsCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  target(0)
{
}

void specManageConnectionsCommand::clear()
{
	if (target)
		delete target ;
	foreach(specGenealogy *item, items)
		delete item ;
	items.clear();
	target = 0 ;
}

specManageConnectionsCommand::~specManageConnectionsCommand()
{
	clear() ;
}

void specManageConnectionsCommand::setItems(const QModelIndex &client, QModelIndexList &servers)
{
	// TODO check if client is metaItem
	clear() ;
	target = new specGenealogy(QModelIndexList() << client) ;
	((specModel*) servers.first().model())->eliminateChildren(servers);
	qSort(servers) ;
	while(!servers.isEmpty())
		items << new specGenealogy(servers) ;
}

bool specManageConnectionsCommand::ok() const
{
	return parentObject() && !items.isEmpty() && target ;
}

void specManageConnectionsCommand::restore()
{
	qDebug() << "restoring connections" ;
	QVector<specModelItem*> pointers = itemPointers() ;
	qDebug() << "got itemPointers" << pointers ;
	specMetaItem* client = targetPointer() ;
	qDebug() << "got target" << client ;
	foreach(specModelItem *pointer, pointers)
		client->connectServer(pointer); // TODO consider removal from list if false is returned.
	qDebug() << "done." ;
}

void specManageConnectionsCommand::take()
{
	QVector<specModelItem*> pointers = itemPointers() ;
	specMetaItem* client = targetPointer() ;
	foreach(specModelItem *pointer, pointers)
		pointer->disconnectClient(client) ;
}

void specManageConnectionsCommand::writeToStream(QDataStream &out) const
{
	if (!ok()) return ; // TODO why check if ok?
	out << qint8(sameModel())
	    << qint32(items.size())
	    << *target ;
	for (int i = 0 ; i < items.size() ; ++i)
		out << *(items[i]) ;
}

void specManageConnectionsCommand::readFromStream(QDataStream &in)
{
	clear() ;
	qint8 isSameModel ;
	qint32 toRead ;
	in >> isSameModel >> toRead ;
	target = new specGenealogy ;
	in >> *target ;
	for (int i = 0 ; i < toRead ; ++i)
	{
		specGenealogy* genealogy = new specGenealogy;
		in >> *genealogy ;
		items << genealogy ;
	}
}

void specManageConnectionsCommand::parentAssigned()
{
	if (!parentObject()) return ;
	specModel *model = sameModel() ?
		(specModel*) (((QAbstractItemView*) parentObject())->model()) :
		(specModel*) (((specPlotWidget*) parentObject()->parent()->parent())->mainView()->model()) ;
	target->setModel((specModel*) (((QAbstractItemView*) parentObject())->model()));
	foreach(specGenealogy* genealogy, items)
		genealogy->setModel(model) ;
}

QVector<specModelItem*> specManageConnectionsCommand::itemPointers() const
{
	QVector<specModelItem*> pointers ;
	foreach(specGenealogy *genealogy,items)
		pointers << genealogy->items() ; // TODO think over
	return pointers ;
}

specMetaItem *specManageConnectionsCommand::targetPointer() const
{
	return (specMetaItem*) (target->firstItem()) ;
}

bool specManageConnectionsCommand::sameModel() const
{
	specModel *targetModel = target->model() ;
	specModel *serverModel = items.first()->model() ; // TODO precautions
	return targetModel == serverModel ;
}
