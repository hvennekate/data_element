#include "specmovecommand.h"
#include "specmodel.h"
#include "specview.h"

void specMoveCommand::moveUnit::pointersToIndexes(specModel *model)
{
	if (!model || items.isEmpty() || parent == 0 || items.first() == 0)
	{
		firstItemIndex.clear();
		parentIndex.clear();
		count = 0 ;
		return ;
	}
	parentIndex = model->hierarchy(parent) ;
	firstItemIndex = model->hierarchy(items.first()) ;
	items.clear();
	parent = 0 ;
}

void specMoveCommand::moveUnit::IndexesToPointers(specModel *model)
{
	if (!model || firstItemIndex.isEmpty())
	{
		items.clear();
		parent = 0 ;
		return ;
	}
	parent = dynamic_cast<specFolderItem*>(model->itemPointer(parentIndex)) ;
	items.clear();
	QVector<int> prefix(firstItemIndex) ;
	for (int i = 0 ; i < count ; ++i)
	{
		items << model->itemPointer(prefix) ;
		prefix.last() ++ ;
	}
	firstItemIndex.clear();
	parentIndex.clear();
	qDebug() << "IndexesToPointers" << count ;
}

void specMoveCommand::moveUnit::moveIt(specModel* model)
{
//	IndexesToPointers(model);
	specFolderItem *oldParent = items.first()->parent() ;
	int oldRow = oldParent->childNo(items.first()) ;
	foreach (specModelItem* item, items)
		item->setParent(0);
	parent->addChildren(items.toList(),row) ;
	row = oldRow ;
	parent = oldParent ;
//	pointersToIndexes(model);
}

QDataStream& operator<<(QDataStream& out, const specMoveCommand::moveUnit& unit)
{
	return out << unit.firstItemIndex << unit.parentIndex << unit.count << unit.row ;
}

QDataStream& operator>>(QDataStream& in, specMoveCommand::moveUnit& unit)
{
	return in >> unit.firstItemIndex >> unit.parentIndex >> unit.count >> unit.row ;
}

specMoveCommand::moveUnit::moveUnit(QModelIndexList &list, const QModelIndex& target, int r, specModel *model)
	: count(0),
	  parent(0)
{
	if (list.isEmpty() || !model) return ;
	int nextRow = list.first().row() ;
	firstItemIndex = model->hierarchy(list.first()) ;
	if (firstItemIndex.isEmpty()) return ;
	parentIndex = model->hierarchy(target) ;
	row = r ;
	const QModelIndex parentModelIndex(list.first().parent()) ;
	while (!list.isEmpty() &&
			list.first().row() == nextRow++ &&
			list.first().parent() == parentModelIndex)
	{
		count ++ ;
		list.takeFirst() ;
	}
}

specMoveCommand::moveUnit::moveUnit()
	: count(0),
	  parent(0),
	  row(0)
{
}

specMoveCommand::specMoveCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

void specMoveCommand::setItems(QModelIndexList &sources, const QModelIndex &target, int row)
{
	if (!refresh()) return ;
	int oldSize = sources.size() ;
	while (!sources.isEmpty())
		moveUnits << moveUnit(sources,target,row+(oldSize - sources.size()),model) ;
}

bool specMoveCommand::refresh()
{
	view = qobject_cast<specView*>(parentObject()) ;
	if (!view) return false;
	model = view->model() ; // TODO put into function
	if (!model) return false;
	return true ;
}

void specMoveCommand::doIt()
{
	if (!refresh()) return ;
	model->signalBeginReset();
	for (QVector<moveUnit>::iterator unit = moveUnits.begin() ; unit != moveUnits.end() ; ++unit)
		unit->IndexesToPointers(model) ;
	for (QVector<moveUnit>::iterator unit = moveUnits.begin() ; unit != moveUnits.end() ; ++unit)
		unit->moveIt(model) ;
	for (QVector<moveUnit>::iterator unit = moveUnits.begin() ; unit != moveUnits.end() ; ++unit)
		unit->pointersToIndexes(model) ;
	model->signalEndReset();
}

void specMoveCommand::undoIt()
{
	doIt() ;
}

void specMoveCommand::writeCommand(QDataStream &out) const
{
	out << moveUnits ;
}

void specMoveCommand::readCommand(QDataStream &in)
{
	in >> moveUnits ;
}
