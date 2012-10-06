#ifndef SPECEXCHANGEFITCURVECOMMAND_H
#define SPECEXCHANGEFITCURVECOMMAND_H

#include "specundocommand.h"
#include "specmetaitem.h"
#include "specgenealogy.h"

class specFitCurve ;

class specExchangeFitCurveCommand : public specUndoCommand
{
private:
	specFitCurve* curve ;
	specGenealogy item ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	void doIt() ;
	void undoIt() ;
	void parentAssigned();

	type typeId() const { return specStreamable::exchangeFitCommand ; }
public:
	explicit specExchangeFitCurveCommand(specUndoCommand *parent = 0) ;
	void setup(const QModelIndex&, specFitCurve*) ;
	~specExchangeFitCurveCommand() ;
};

#endif // SPECEXCHANGEFITCURVECOMMAND_H
