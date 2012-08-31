#include "specmovecommand.h"
#include "specmodel.h"
#include "specview.h"

void specMoveCommand::setItems(QModelIndexList &sources, const QModelIndex &target, int row)
{
	number = 0 ;
	targetIndex << row ;//< 0 ? 0 : row ; // TODO maybe change to last row.
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
	specModel* model = ((specView*) parentObject())->model() ; // TODO put into function
	specModelItem* targetFolder = (specFolderItem*) model->itemPointer(targetIndex.mid(1)) ;
	if (!targetFolder->isFolder())
		targetFolder = targetFolder->parent() ;
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
	foreach (specModelItem* item, items)
		if (item->parent() == targetFolder && ((specFolderItem*) targetFolder)->childNo(item) < targetIndex[0])
			--targetIndex[0] ;
}

specMoveCommand::specMoveCommand(specUndoCommand *parent)
	:specUndoCommand(parent),
	  number(0)
{
}

void specMoveCommand::doIt()
{
	if (!parentObject()) return ;
	specModel* model = ((specView*) parentObject())->model() ; // TODO put into function
	model->signalBeginReset();
	specModelItem* target = (specFolderItem*) model->itemPointer(targetIndex.mid(1)) ;
	if (!target->isFolder())
		target = target->parent() ;
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
	// halting parents
	QVector<specFolderItem*> parents ;
	foreach(specModelItem* item, items)
		if (!parents.contains(item->parent()))
			parents << item->parent() ;
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(true) ;
	// Precautions if moving within the same folder
	foreach(specModelItem* item, items)
		item->setParent(0) ;
	qDebug() << "moving to" << targetIndex ;
	if (targetIndex[0] < 0) targetIndex[0] = target->children() ;
	((specFolderItem*) target)->addChildren(items,targetIndex[0]) ;
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(false) ;
	model->signalEndReset();
	sourceIndex = model->hierarchy(target) ;
}

void specMoveCommand::undoIt()
{
	specModel* model = ((specView*) parentObject())->model() ; // TODO put into function
	model->signalBeginReset();
	specModelItem* target = (specFolderItem*)model->itemPointer(sourceIndex) ;
	if (!target->isFolder()) target = target->parent() ;
	((specFolderItem*) target)->haltRefreshes(true) ;
	QVector<specFolderItem*> parents ;
	QList<specModelItem*> items ;
	int row = targetIndex[0] ;
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
		count += sourceIndexes[i].second ;
	}
	foreach(specFolderItem* parent, parents)
		parent->haltRefreshes(false) ;
	((specFolderItem*) target)->haltRefreshes(false) ;
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
