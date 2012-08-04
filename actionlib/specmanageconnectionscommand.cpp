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
	QVector<specModelItem*> pointers = itemPointers() ;
	specMetaItem* client = targetPointer() ;
	foreach(specModelItem *pointer, pointers)
		client->connectServer(pointer); // TODO consider removal from list if false is returned.
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
	out << qint32(items.size())
	    << *target ;
	for (int i = 0 ; i < items.size() ; ++i)
		out << *(items[i]) ;
}

void specManageConnectionsCommand::readFromStream(QDataStream &in)
{
	clear() ;
	qint32 toRead ;
	in >> toRead ;
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
	if (!target) return ;

	specModel *thisModel = (specModel*) (((QAbstractItemView*) parentObject())->model()) ,
			  *otherModel= (specModel*) (((specPlotWidget*) (parentObject()->parent()->parent()))->mainView()->model()) ;
	foreach(specGenealogy* genealogy, items)
		genealogy->setModel(sameModel(genealogy) ? thisModel : otherModel) ;
	target->setModel(thisModel) ;
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

bool specManageConnectionsCommand::sameModel(specGenealogy* item) const
{
	if (!target || !item) return false ;
	// TODO check
	return target->model() == item->model() ;
}
