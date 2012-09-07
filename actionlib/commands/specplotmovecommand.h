#ifndef SPECPLOTMOVECOMMAND_H
#define SPECPLOTMOVECOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"
#include "specmodel.h"

class specPlotMoveCommand : public specUndoCommand
{
private:
	double slope, offset, scale, shift ;
	specGenealogy *items ; // TODO define type to hold reference to one single item.
	void undoIt() ;
	void doIt() ;
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::movePlotCommandId ;}
public:
	specPlotMoveCommand(specUndoCommand *parent = 0);
	bool mergeWith(const QUndoCommand* other) ;
	void setItem(QModelIndex) ;
	void setCorrections(double xShift, double yOffset, double ySlope, double yScale) ;
	bool mergeable(const specUndoCommand* other) ;
};

#endif // SPECPLOTMOVECOMMAND_H
