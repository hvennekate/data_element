#ifndef SPECEXCHANGEFITCURVECOMMAND_H
#define SPECEXCHANGEFITCURVECOMMAND_H

#include "specsingleitemcommand.h"
#include "specmetaitem.h"

class specFitCurve ;

class specExchangeFitCurveCommand : public specSingleItemCommand<specMetaItem>
{
private:
	specFitCurve* curve ;
	void writeCommand(QDataStream& out) const ;
	void readCommand(QDataStream& in) ;
	void doIt() ;
	void undoIt() ;
	type typeId() const { return specStreamable::exchangeFitCommand ; }
public:
	explicit specExchangeFitCurveCommand(specUndoCommand* parent = 0) ;
	void setup(specModelItem*, specFitCurve*) ;
	~specExchangeFitCurveCommand() ;
};

#endif // SPECEXCHANGEFITCURVECOMMAND_H
