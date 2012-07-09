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

void specManageConnectionsCommand::write(specOutStream &out) const
{
	if (!ok) return ;
	out.startContainer(spec::ManageConnectionsCommandId) ;
	out << qint8(sameModel()) ;
	out << qint32(items.size()) ;
//	if (ok()) 	// TODO why check if ok?
	target->write(out) ;
	for (int i = 0 ; i < items.size() ; ++i)
		items[i]->write(out) ;
	out.stopContainer();
	return out ;
}

bool specManageConnectionsCommand::read(specInStream &in)
{
	clear() ;
	if (!in.expect(spec::ManageConnectionsCommandId)) return false ;
	qint8 isSameModel ;
	qint32 toRead ;
	in >> isSameModel >> toRead ;
	specModel *model = isSameModel ?
				(specModel*) (((QAbstractItemView*) parentObject())->model()) :
				(specModel*) (((specPlotWidget*) parentObject()->parent()->parent())->mainView()->model()) ; // TODO this is highly dangerous!
	target = new specGenealogy((specModel*) (((QAbstractItemView*) parentObject())->model()),in) ;
	for (int i = 0 ; i < toRead ; ++i)
		items << new specGenealogy(model,in) ;
	return !in.next() ;
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
