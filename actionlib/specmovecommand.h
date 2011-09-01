#ifndef SPECMOVECOMMAND_H
#define SPECMOVECOMMAND_H

#include "specundocommand.h"

class specMoveCommand : public specUndoCommand
{
private:
	QVector<QPair<QVector<int>, int > > source ;
	QVector<int>
public:
    explicit specMoveCommand(const QModelIndexList& sources, const QModelIndex& target, );
};

#endif // SPECMOVECOMMAND_H
