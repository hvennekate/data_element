#include "specmovecommand.h"
#include "specmodel.h"
#include "specview.h"

specMoveCommand::specMoveCommand(QModelIndexList &sources, const QModelIndex &target,int row, specUndoCommand *parent)
	: specUndoCommand(parent)
{
	number = 0 ;
	targetIndex << row ;//< 0 ? 0 : row ; // TODO maybe change to last row.
	qDebug("##### row %d",row) ;
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

specMoveCommand::specMoveCommand(specUndoCommand *parent)
	:specUndoCommand(parent),
	  number(0)
{
}

void specMoveCommand::doIt()
{
	qDebug("doing move action") ;
	if (!parentObject()) return ;
	specModel* model = ((specView*) parentObject())->model() ; // TODO put into function
	qDebug("got model") ;
	model->signalBeginReset();
	qDebug("getting target") ;
	specModelItem* target = (specFolderItem*) model->itemPointer(targetIndex.mid(1)) ;
	qDebug() << "verifying that target is a folder %d" << target << targetIndex ;
	if (!target->isFolder())
	{
		qDebug("not a folder") ;
		target = target->parent() ;
		qDebug("parent: %d",target) ;
	}
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
	if (targetIndex[0] < 0) targetIndex[0] = target->children() ;
	((specFolderItem*) target)->addChildren(items,targetIndex[0]) ;
	qDebug("moved items, resetting refreshes") ;
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(false) ;
	model->signalEndReset();
	sourceIndex = model->hierarchy(target) ;
	qDebug("done with move action") ;
}

void specMoveCommand::undoIt()
{
	specModel* model = ((specView*) parentObject())->model() ; // TODO put into function
	model->signalBeginReset();
	qDebug("## getting target") ;
	specModelItem* target = (specFolderItem*)model->itemPointer(sourceIndex) ;
	qDebug("got target %d",target) ;
	if (!target->isFolder()) target = target->parent() ;
	((specFolderItem*) target)->haltRefreshes(true) ;
	qDebug("## getting parents and items") ;
	QVector<specFolderItem*> parents ;
	QList<specModelItem*> items ;
	int row = targetIndex[0] ;
	qDebug("## getting item pointers %d %d",target,row) ;
	for (int i = 0 ; i < number ; ++i)
		items << ((specFolderItem*) target)->child(i+row) ;
	qDebug("## got items") ;
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
		qDebug("adding children count: %d row: %d from list pos: %d pointer: %d parent: %d",sourceIndexes[i].second,sourceIndexes[i].first[0],count,items[count],parent) ;
		parent->addChildren(items.mid(count,sourceIndexes[i].second),
				    sourceIndexes[i].first[0]) ;
		count += sourceIndexes[i].second ;
		qDebug("adding done") ;
	}
	qDebug("## moved items back") ;
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(false) ;
	((specFolderItem*) target)->haltRefreshes(false) ;
	qDebug("## almost done") ;
	model->signalEndReset();
}

void specMoveCommand::writeToStream(QDataStream &out) const
{
	out << sourceIndexes << number << targetIndex << sourceIndex ;
}

void specMoveCommand::readFromStream(QDataStream &in)
{
	in >> sourceIndexes >> number >> targetIndex >> sourceIndex ;
}
