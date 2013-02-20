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

void specMoveCommand::moveUnit::indexesToPointers(specModel *model)
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
		prefix.first() ++ ; // address vectors are little endian
	}
	firstItemIndex.clear();
	parentIndex.clear();
}

void specMoveCommand::moveUnit::moveIt()
{
	specFolderItem *oldParent = items.first()->parent() ;
	int oldRow = oldParent->childNo(items.first()) ;
	foreach (specModelItem* item, items)
		item->setParent(0);
	parent->addChildren(items.toList(),row) ;
	row = oldRow ;
	parent = oldParent ;
}

QDataStream& operator<<(QDataStream& out, const specMoveCommand::moveUnit& unit)
{
	return out << unit.firstItemIndex << unit.parentIndex << unit.count << unit.row ;
}

QDataStream& operator>>(QDataStream& in, specMoveCommand::moveUnit& unit)
{
	return in >> unit.firstItemIndex >> unit.parentIndex >> unit.count >> unit.row ;
}

specMoveCommand::moveUnit::moveUnit(QModelIndexList &list, const QModelIndex& target, int &r, specModel *model)
	: parent(0),
	  count(0),
	  row(r)
{
	if (list.isEmpty() || !model) return ;
	int nextRow = list.first().row() ;
	int firstRow = nextRow ;
	firstItemIndex = model->hierarchy(list.first()) ;
	if (firstItemIndex.isEmpty()) return ;
	parentIndex = model->hierarchy(target) ;
	const QModelIndex parentModelIndex(list.first().parent()) ;
	while (!list.isEmpty() &&
			list.first().row() == nextRow++ &&
			list.first().parent() == parentModelIndex)
	{
		count ++ ;
		list.takeFirst() ;
	}
	if (parentModelIndex == target && row > firstRow) // moving within same parent
		row = qMax(row - count, firstRow) ;
	r = row + count ; // set next insert position
}

specMoveCommand::moveUnit::moveUnit()
	: parent(0),
	  count(0),
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
	while (!sources.isEmpty())
		moveUnits << moveUnit(sources,target,row,model) ;
}

bool specMoveCommand::refresh()
{
	return (model = qobject_cast<specModel*>(parentObject())) ; // TODO put into function
}

bool specMoveCommand::prepare()
{
	if (!refresh()) return false ;
	for (QVector<moveUnit>::iterator i = moveUnits.begin() ; i != moveUnits.end() ; ++i)
		i->indexesToPointers(model) ;
	model->signalBeginReset();
	return true ;
}

void specMoveCommand::finish()
{
	model->signalEndReset();
	for (QVector<moveUnit>::iterator i = moveUnits.begin() ; i != moveUnits.end() ; ++i)
		i->pointersToIndexes(model) ;
}

void specMoveCommand::doIt()
{
	if (!prepare()) return ;
	for (int i = 0 ; i < moveUnits.size() ; ++i)
		moveUnits[i].moveIt() ;
	finish() ;
}

void specMoveCommand::undoIt()
{
	if (!prepare()) return ;
	for (int i = moveUnits.size() ; i > 0 ; --i)
		moveUnits[i-1].moveIt() ;
	finish() ;
}

void specMoveCommand::writeCommand(QDataStream &out) const
{
	out << moveUnits ;
}

void specMoveCommand::readCommand(QDataStream &in)
{
	in >> moveUnits ;
}
