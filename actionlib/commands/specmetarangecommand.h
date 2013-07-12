#ifndef SPECMETARANGECOMMAND_H
#define SPECMETARANGECOMMAND_H

#include "specsingleitemcommand.h"

class specMetaRangeCommand : public specSingleItemCommand<specMetaItem>
{
private:
	double oldX, oldY, newX, newY ;
	int rangeNo, pointNo, variableNo ;
	void undoIt() ;
	void doIt() ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::metaRangeCommand ; }
public:
	specMetaRangeCommand(specUndoCommand* parent = 0);
	bool mergeWith(const QUndoCommand *other) ;
	void setItem(specMetaItem*, int VariableIndex, int rangeIndex, int pointNo, double newX, double newY) ;
	bool mergeable(const specUndoCommand *other) ;
};

#endif // SPECMETARANGECOMMAND_H
