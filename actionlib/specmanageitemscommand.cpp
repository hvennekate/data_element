#include "specmanageitemscommand.h"
#include <QDebug>
#include <QAbstractItemView>

specManageItemsCommand::specManageItemsCommand(specUndoCommand *parent) :
	specUndoCommand(parent),
	items()
{
}

void specManageItemsCommand::clear()
{
	foreach(specGenealogy* item, items)
		delete item ;
	items.clear();
}

void specManageItemsCommand::setItems(QModelIndexList & indexes)
{
	if (indexes.isEmpty()) return ;
	clear() ;
	((specModel*) indexes.first().model())->eliminateChildren(indexes) ;
	qSort(indexes) ;
	qDebug("adding genealogy") ;
	while (!indexes.isEmpty())
		items << new specGenealogy(indexes) ;
}

bool specManageItemsCommand::ok()
{
	return parentObject() && !items.isEmpty() ;
}

void specManageItemsCommand::prepare() // TODO make this the model's task
{
	qDebug("signalling begin reset") ;
	items.first()->model()->signalBeginReset();
	qDebug("specManageItemsCommand:  halting refreshes %d", items.size())  ;
	for(int i = 0 ; i < items.size() ; ++i)
	{
		qDebug() << "manage items command: parent of genealogy: " << items[i]->parent() ;
		if (!items[i]->parent())
			if (!items[i]->seekParent())
				continue ;
		qDebug() << "manage items command: parent of genealogy: " << items[i]->parent() ;
		items[i]->parent()->haltRefreshes(true) ;
	}
}

void specManageItemsCommand::finish()
{
	qDebug("reenabling refreshes") ;
	for (int i = 0 ; i < items.size() ; ++i)
	{
		qDebug() << items[i]->parent() ;
		items[i]->parent()->haltRefreshes(false) ;
	}
	qDebug("signalling reset done") ;
	items.first()->model()->signalEndReset();
}

void specManageItemsCommand::take()
{
	if (!ok()) return ;
	qDebug("preparing model and parents") ;
	prepare() ;
	qDebug("removing genealogies") ;
	for(int i = 0 ; i < items.size() ; ++i)
		items[i]->takeItems();
	finish() ;
}

void specManageItemsCommand::restore()
{
	if (!ok()) return ;
	if (!parentObject() || items.isEmpty()) return ;
	prepare() ;
	for(int i = 0 ; i < items.size() ; i++)
		items[i]->returnItems();
	finish() ;
}

void specManageItemsCommand::writeToStream(QDataStream &out) const
{
	out << qint32(items.size()) ;
	foreach(specGenealogy* genealogy, items)
		out << *genealogy ;
}

void specManageItemsCommand::readFromStream(QDataStream &in)
{
	qint32 toRead ;
	in >> toRead ;
	for (int i = 0 ; i < toRead ; ++i)
	{
		specGenealogy *genealogy = new specGenealogy ;
		in >> *genealogy ;
		items << genealogy ;
	}
}

specManageItemsCommand::~specManageItemsCommand()
{
	clear() ;
}

void specManageItemsCommand::parentAssigned()
{
	if (!parentObject()) return ;
	specModel *model = (specModel*) (((QAbstractItemView*) parentObject())->model()) ;
	foreach(specGenealogy* genealogy, items)
		genealogy->setModel(model) ;
}
