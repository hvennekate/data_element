#ifndef SPECPLOTMOVECOMMAND_H
#define SPECPLOTMOVECOMMAND_H

#include "specsingleitemcommand.h"

class specPlotMoveCommand : public specSingleItemCommand<specModelItem>
{
private:
	double slope, offset, scale, shift ;
	specUndoCommand* alternativeCommand ;
	void undoIt() ;
	void doIt() ;
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	type typeId() const ;
	void generateDescription() ;
	QString description() const ;
public:
	specPlotMoveCommand(specUndoCommand *parent = 0);
};

#endif // SPECPLOTMOVECOMMAND_H
