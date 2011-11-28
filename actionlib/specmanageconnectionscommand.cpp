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
	return parentWidget() && !items.isEmpty() && target ;
}

void specManageConnectionsCommand::restore()
{
	QVector<specModelItem*> pointers = itemPointers() ;
	specMetaItem* client = targetPointer() ;
	foreach(specModelItem *pointer, pointers)
		client->connectServer(pointer) ; // TODO consider removal from list if false is returned.
}

void specManageConnectionsCommand::take()
{
	QVector<specModelItem*> pointers = itemPointers() ;
	specMetaItem* client = targetPointer() ;
	foreach(specModelItem *pointer, pointers)
		client->disconnectServer(pointer) ;
}

QDataStream &specManageConnectionsCommand::write(QDataStream &out) const
{
	out << qint8(sameModel()) ;
	out << qint32(items.size()) ;
	for (int i = 0 ; i < items.size() ; ++i)
		items[i]->write(out) ;
	if (ok())
		target->write(out) ;
	return out ;
}

QDataStream &specManageConnectionsCommand::read(QDataStream &in)
{
	clear() ;
	qint8 isSameModel ;
	qint32 toRead ;
	in >> isSameModel >> toRead ;
	specModel *model = isSameModel ?
				(specModel*) (((QAbstractItemView*) parentWidget())->model()) :
				(specModel*) (((specPlotWidget*) parentWidget()->parent()->parent())->mainView()->model()) ; // TODO this is highly dangerous!
	for (int i = 0 ; i < toRead ; ++i)
		items << new specGenealogy(model,in) ;
	target = new specGenealogy((specModel*) (((QAbstractItemView*) parentWidget())->model()),in) ;
	return in ;
}

QVector<specModelItem*> specManageConnectionsCommand::itemPointers() const
{
	QVector<specModelItem*> pointers ;
	foreach(specGenealogy *genealogy,items)
		pointers << genealogy->items().toVector() ; // TODO think over
	return pointers ;
}

specMetaItem *specManageConnectionsCommand::targetPointer() const
{
	return (specMetaItem*) (target->items().first()) ;
}

bool specManageConnectionsCommand::sameModel() const
{
	specModel *targetModel = target->model() ;
	specModel *serverModel = items.first()->model() ; // TODO precautions
	return targetModel == serverModel ;
}
