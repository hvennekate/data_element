#ifndef SPECMETARANGECOMMAND_H
#define SPECMETARANGECOMMAND_H
#include "specundocommand.h"
#include "specgenealogy.h"

class specMetaRangeCommand : public specUndoCommand
{
private:
	double oldX, oldY, newX, newY ;
	int rangeNo, pointNo, variableNo ;
	specGenealogy *item ; // TODO single item type
	void undoIt() ;
	void doIt() ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::metaRangeCommand ; }
	void parentAssigned();
public:
	specMetaRangeCommand(specUndoCommand* parent = 0);
	bool mergeWith(const QUndoCommand *other) ;
	void setItem(QModelIndex, int VariableIndex, int rangeIndex, int pointNo, double newX, double newY) ;
	bool mergeable(const specUndoCommand *other) ;
};

#endif // SPECMETARANGECOMMAND_H
