#include "specmovecommand.h"
#include "specmodel.h"
#include "specview.h"

specMoveCommand::specMoveCommand(QModelIndexList &sources, const QModelIndex &target, specUndoCommand *parent)
	: specUndoCommand(parent)
{
	number = 0 ;
//	targetIndex << row ;
	targetIndex << specModel::hierarchy(target) ;
	while(!sources.isEmpty())
	{
		QModelIndex current = sources.takeFirst() ;
		sourceIndexes << QPair<QVector<int>,int>(specModel::hierarchy(current),1) ;
		int row = current.row() ;
		current = current.parent() ;
		int count = 1 ;
		while (!sources.isEmpty()
				&& sources.first().parent() == current
				&& sources.first().row() == ++row)
		{
			sources.takeFirst() ;
			++ count;
		}
		sourceIndexes.last().second = count ;
		number += count ;
	}
}

void specMoveCommand::redo()
{
	qDebug("doing move action") ;
	if (!pW) return ;
	specModel* model = ((specView*) pW)->model() ; // TODO put into function
	qDebug("got model") ;
	model->signalBeginReset();
	qDebug("getting target") ;
	specModelItem* target = (specFolderItem*) model->itemPointer(targetIndex) ;
	qDebug("verifying that target is a folder") ;
	if (!target->isFolder()) target = target->parent() ;
	qDebug("got target") ;
	QList<specModelItem*> items ;
	for (int i = 0 ; i < sourceIndexes.size() ; ++i)
	{
		QVector<int> indexes = sourceIndexes[i].first ;
		for (int j = 0 ; j < sourceIndexes[i].second ; ++j)
		{
			items << model->itemPointer(indexes) ;
			++ indexes[0] ;
		}

	}
	qDebug("got items") ;
	QVector<specFolderItem*> parents ;
	foreach(specModelItem* item, items)
		if (!parents.contains(item->parent()))
			parents << item->parent() ;
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(true) ;
	qDebug("halted parents") ;
	((specFolderItem*) target)->addChildren(items,targetIndex[0]) ;
	qDebug("moved items, resetting refreshes") ;
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(false) ;
	model->signalEndReset();
	qDebug("done with move action") ;
}

void specMoveCommand::undo()
{
	if (!pW) return ;
	specModel* model = ((specView*) pW)->model() ; // TODO put into function
	model->signalBeginReset();
	specModelItem* target = (specFolderItem*)model->itemPointer(targetIndex) ;
	if (!target->isFolder()) target = target->parent() ;
	QVector<specFolderItem*> parents ;
	((specFolderItem*) target)->haltRefreshes(true) ;
	QList<specModelItem*> items ;
	int row = targetIndex.first() ;
	for (int i = 0 ; i < number ; ++i)
		items << ((specFolderItem*) target)->child(i+row) ;
	int count = 0 ;
	for (int i = 0 ; i < sourceIndexes.size() ; ++i)
	{
		QVector<int> indexes = sourceIndexes[i].first.mid(1) ;
		specFolderItem* parent = (specFolderItem*) model->itemPointer(indexes) ;
		if (!parents.contains(parent))
		{
			parents << parent ;
			parent->haltRefreshes(true) ;
		}
		parent->addChildren(items.mid(count,sourceIndexes[i].second),
				    sourceIndexes[i].first[0]) ;
	}

	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(false) ;
	((specFolderItem*) target)->haltRefreshes(false) ;
	model->signalEndReset();
}

QDataStream& specMoveCommand::write(QDataStream &out)
{
	return out ;
}

QDataStream& specMoveCommand::read(QDataStream &in)
{
	return in ;
}
