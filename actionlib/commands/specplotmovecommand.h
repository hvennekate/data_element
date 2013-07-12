#ifndef SPECPLOTMOVECOMMAND_H
#define SPECPLOTMOVECOMMAND_H

#include "specsingleitemcommand.h"

class specPlotMoveCommand : public specSingleItemCommand<specModelItem>
{
private:
	double slope, offset, scale, shift ;
	void undoIt() ;
	void doIt() ;
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::movePlotCommandId ;}
	void generateDescription() ;
	QString description() const ;
public:
	specPlotMoveCommand(specUndoCommand *parent = 0);
	bool mergeWith(const QUndoCommand* other) ;
	void setCorrections(double xShift, double yOffset, double ySlope, double yScale) ;
	bool mergeable(const specUndoCommand* other) ;
};

#endif // SPECPLOTMOVECOMMAND_H
