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
	int number ;
	QVector<int> targetIndex ;
	QVector<int> getIndexVector(QModelIndex) ; // TODO make either separate class or static public function
	void refreshPointers() ;
public:
	specMoveCommand(QModelIndexList& sources, const QModelIndex& target, specUndoCommand* parent=0) ;
	explicit specMoveCommand(QDataStream&) ;
	void redo() ;
	void undo() ;
	QDataStream& write(QDataStream &) ;
	QDataStream& read(QDataStream &) ;
};

#endif // SPECMOVECOMMAND_H
