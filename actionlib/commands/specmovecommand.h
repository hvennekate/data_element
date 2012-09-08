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
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::moveItemsCommandId ; }
public:
	void setItems(QModelIndexList& sources, const QModelIndex& target, int row) ;
	specMoveCommand(specUndoCommand* parent = 0) ;
};

#endif // SPECMOVECOMMAND_H
