#ifndef SPECEXCHANGEDATACOMMAND_H
#define SPECEXCHANGEDATACOMMAND_H

#include "specsingleitemcommand.h"
#include "specdataitem.h"

class specExchangeDataCommand : public specSingleItemCommand<specDataItem>
{
private:
	QVector<specDataPoint> data ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in);
	void doIt();
	void undoIt() ;

	type typeId() const { return specStreamable::exchangeDataCommandId  ; }
public:
	explicit specExchangeDataCommand(specUndoCommand *parent = 0) ;
	void setItem(specModelItem*, const QVector<specDataPoint>& newData) ;
};

#endif // SPECEXCHANGEDATACOMMAND_H
