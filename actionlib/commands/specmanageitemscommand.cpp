#include "specmanageitemscommand.h"
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
	while (!indexes.isEmpty())
		items << new specGenealogy(indexes) ;
}

bool specManageItemsCommand::ok()
{
	return parentObject() && !items.isEmpty() ;
}

void specManageItemsCommand::prepare() // TODO make this the model's task
{
	items.first()->model()->signalBeginReset();
	for(int i = 0 ; i < items.size() ; ++i)
	{
		if (!items[i]->parent())
			if (!items[i]->seekParent())
				continue ;
		items[i]->parent()->haltRefreshes(true) ;
	}
}

void specManageItemsCommand::finish()
{
	for (int i = 0 ; i < items.size() ; ++i)
	{
		items[i]->parent()->haltRefreshes(false) ;
	}
	items.first()->model()->signalEndReset();
}

void specManageItemsCommand::take()
{
	if (!ok()) return ;
	prepare() ;
	qDebug() << "command" << this << "taking items:" << items ;
	for(int i = 0 ; i < items.size() ; ++i)
		items[i]->takeItems();
	finish() ;
}

void specManageItemsCommand::restore()
{
	if (!ok()) return ;
	if (!parentObject() || items.isEmpty()) return ;
	prepare() ;
	qDebug() << "command" << this << "restoring items:" << items ;
	for(int i = 0 ; i < items.size() ; i++)
		items[i]->returnItems();
	finish() ;
}

void specManageItemsCommand::writeCommand(QDataStream &out) const
{
	out << qint32(items.size()) ;
	foreach(specGenealogy* genealogy, items)
		out << *genealogy ;
}

void specManageItemsCommand::readCommand(QDataStream &in)
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
	specModel *model = (specModel*) parentObject() ;
	foreach(specGenealogy* genealogy, items)
		genealogy->setModel(model) ;
}
