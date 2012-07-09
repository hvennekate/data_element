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
	QVector<int> sourceIndex ;
public:
	specMoveCommand(QModelIndexList& sources, const QModelIndex& target, int row, specUndoCommand* parent=0) ;
	specMoveCommand(specUndoCommand* parent = 0) ;
	void redo() ;
	void undo() ;
	int id() const { return spec::moveItemsId ; }
	void write(specOutStream &) const ;
	bool read(specInStream&) ;
};

#endif // SPECMOVECOMMAND_H
