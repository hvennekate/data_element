#ifndef SPECPLOTMOVECOMMAND_H
#define SPECPLOTMOVECOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"
#include "specmodel.h"

class specPlotMoveCommand : public specUndoCommand
{
	double slope, offset, scale, shift ;
	specGenealogy items ; // TODO define type to hold reference to one single item.
	specModel *model ;
public:
	specPlotMoveCommand(specUndoCommand *parent = 0);
	void undo() ;
	void redo() ;
	bool merge(specPlotMoveCommand* other) ;
	void setItem(QModelIndex&) ;
	void setCorrections(double xShift, double yOffset, double ySlope, double yScale) ;
	int id() const { return spec::movePlotId ;}
};

#endif // SPECPLOTMOVECOMMAND_H
