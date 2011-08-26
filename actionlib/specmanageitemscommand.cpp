#include "specmanageitemscommand.h"
#include <QDebug>

specManageItemsCommand::specManageItemsCommand(specUndoCommand *parent) :
	specUndoCommand(parent)
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
	return ! items.isEmpty() ;
}

void specManageItemsCommand::prepare() // TODO make this the model's task
{
	qDebug("signalling begin reset") ;
	items.first()->model()->signalBeginReset();
	qDebug("halting refreshes") ;
	for(int i = 0 ; i < items.size() ; ++i)
	{
		qDebug() << items[i]->parent() ;
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
	qDebug("preparing model and parents") ;
	prepare() ;
	qDebug("removing genealogies") ;
	for(int i = 0 ; i < items.size() ; ++i)
		items[i]->takeItems();
	finish() ;
}

void specManageItemsCommand::restore()
{
	prepare() ;
	for(int i = 0 ; i < items.size() ; i++)
		items[i]->returnItems();
	finish() ;
}
