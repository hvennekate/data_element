#ifndef SPECPLOTMOVECOMMAND_H
#define SPECPLOTMOVECOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"
#include "specmodel.h"

class specPlotMoveCommand : public specUndoCommand
{
	double slope, offset, scale, shift ;
	specGenealogy *items ; // TODO define type to hold reference to one single item.
public:
	specPlotMoveCommand(specUndoCommand *parent = 0);
	void undo() ;
	void redo() ;
	bool mergeWith(const QUndoCommand* other) ;
	void setItem(QModelIndex) ;
	void setCorrections(double xShift, double yOffset, double ySlope, double yScale) ;
	int id() const { return spec::movePlotId ;}

	QDataStream& write(QDataStream &out) ;
	QDataStream& read(QDataStream &in) ;
};

#endif // SPECPLOTMOVECOMMAND_H
