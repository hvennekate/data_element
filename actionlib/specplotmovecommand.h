#ifndef SPECPLOTMOVECOMMAND_H
#define SPECPLOTMOVECOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specPlotMoveCommand : public specUndoCommand
{
	double slope, offset, scale, shift ;
	specGenealogy items ;
public:
    specPlotMoveCommand(specUndoCommand *parent = 0);
    void undo() ;
    void redo() ;
    int id() const { return spec::movePlotId ;}
};

#endif // SPECPLOTMOVECOMMAND_H
