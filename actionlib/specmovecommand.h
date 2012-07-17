#ifndef SPECMOVECOMMAND_H
#define SPECMOVECOMMAND_H

#include "specundocommand.h"
#include "specmodelitem.h"
#include <QModelIndex>
#include <QVector>

class specMoveCommand : public specUndoCommand
{
private:
	QVector<QPair<QVector<int>, int > > sourceIndexes ;
	qint32 number ;
	QVector<int> targetIndex ;
	QVector<int> sourceIndex ;
	void doIt() ;
	void undoIt() ;
	void writeToStream(QDataStream &out) const;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::moveItemsCommandId ; }
public:
	specMoveCommand(QModelIndexList& sources, const QModelIndex& target, int row, specUndoCommand* parent=0) ;
	specMoveCommand(specUndoCommand* parent = 0) ;
};

#endif // SPECMOVECOMMAND_H
