#ifndef SPECEXCHANGEDATACOMMAND_H
#define SPECEXCHANGEDATACOMMAND_H

#include "specundocommand.h"
#include "specdataitem.h"
#include "specgenealogy.h"

class specExchangeDataCommand : public specUndoCommand
{
private:
	specGenealogy *item ;
	QVector<specDataPoint> data ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in);
	void doIt();
	void undoIt() ;
	type typeId() const { return specStreamable::exchangeDataCommandId  ; }
public:
	explicit specExchangeDataCommand(specUndoCommand *parent = 0) ;
	void setItem(const QModelIndex&, const QVector<specDataPoint>& newData) ;
};

#endif // SPECEXCHANGEDATACOMMAND_H
