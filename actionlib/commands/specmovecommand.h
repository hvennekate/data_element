#ifndef SPECMOVECOMMAND_H
#define SPECMOVECOMMAND_H

#include "specundocommand.h"
#include "specmodelitem.h"
#include <QModelIndex>
#include <QVector>

class specModel ;
class specView ;

class specMoveCommand : public specUndoCommand
{
public:
	class moveUnit
	{
	private:
		QVector<int> firstItemIndex ;
		QVector<int> parentIndex ;
		QVector<specModelItem*> items ;
		specFolderItem* parent ;
		int count ;
		int row ;
	public:
		void pointersToIndexes(specModel*) ;
		void indexesToPointers(specModel*) ;
		moveUnit(QModelIndexList&, const QModelIndex& target, int& r, specModel*) ;
		moveUnit() ;
		void moveIt() ;
		friend QDataStream& operator<< (QDataStream&, const moveUnit&) ;
		friend QDataStream& operator>> (QDataStream&, moveUnit&) ;
	};
private:
	void doIt() ;
	void undoIt() ;
	void writeCommand(QDataStream& out) const;
	void readCommand(QDataStream& in) ;
	void finish() ;
	bool prepare() ;
	type typeId() const { return specStreamable::moveItemsCommandId ; }

	QVector<moveUnit> moveUnits ;
	bool refresh() ;
	specView* view ;
	specModel* model ;
public:
	void setItems(QModelIndexList& sources, const QModelIndex& target, int row) ;
	specMoveCommand(specUndoCommand* parent = 0) ;
};

QDataStream& operator<< (QDataStream&, const specMoveCommand::moveUnit&) ;
QDataStream& operator>> (QDataStream&, specMoveCommand::moveUnit&) ;

#endif // SPECMOVECOMMAND_H
