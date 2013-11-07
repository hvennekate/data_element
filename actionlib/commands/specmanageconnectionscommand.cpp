#include "specmanageconnectionscommand.h"
#include "specmetaitem.h"
#include "specmetamodel.h"

specManageConnectionsCommand::specManageConnectionsCommand(specUndoCommand* parent)
	: specMultipleItemCommand(parent),
	  targetModel(0)
{
}

void specManageConnectionsCommand::setItems(specMetaItem* client, QList<specModelItem*> servers)
{
	if(!parentObject()) return ;
	if(!targetModel)
		setParentObject(parentObject());  // Cannot be done by specUndoCommand's constructor
	// -> polymorphism not yet complete
	processServers(client, servers) ;
	target = specGenealogy(client, targetModel) ;
	qSort(servers.begin(), servers.end(), specModel::lessThanItemPointer) ;
	specMultipleItemCommand::setItems(servers) ;
}

void specManageConnectionsCommand::restore()
{
	specMetaItem* client = targetPointer() ;
	foreach(specModelItem * pointer, itemPointers())
	client->connectServer(pointer);  // TODO consider removal from list if false is returned.
}

void specManageConnectionsCommand::take()
{
	specMetaItem* client = targetPointer() ;
	foreach(specModelItem * pointer, itemPointers())
	pointer->disconnectClient(client) ;
}

void specManageConnectionsCommand::writeCommand(QDataStream& out) const
{
	out << itemCount() << target ;
	writeItems(out) ;
}

void specManageConnectionsCommand::readCommand(QDataStream& in)
{
	qint32 toRead ;
	in >> toRead >> target ;
	readItems(in, toRead) ;
}

void specManageConnectionsCommand::parentAssigned()
{
	if(!parentObject()) return ;

	/// This section is responsible for the exclusive character of this class (i.e.
	/// only dataItems -> metaItem)
	// Check if we have been assigned a metaModel as parentObject (wrong)
	targetModel = qobject_cast<specMetaModel*> (parentObject()) ;
	if(targetModel)  // yes, so we need to change that.
	{
		setParentObject(targetModel->getDataModel()) ;
		// here, we will be called again, now with the correct parrentObject
		return ;
	}

	targetModel = model()->getMetaModel() ;
	target.setModel(targetModel);
	specMultipleItemCommand::parentAssigned() ;
}

specMetaItem* specManageConnectionsCommand::targetPointer()
{
	return (specMetaItem*)(target.firstItem()) ;
}
