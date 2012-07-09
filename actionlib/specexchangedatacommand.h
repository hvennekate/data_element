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
public:
	explicit specExchangeDataCommand(specUndoCommand *parent = 0) ;
	void setItem(const QModelIndex&, const QVector<specDataPoint>& newData) ;
	void write(specOutStream &out) const ;
	bool read(specInStream &in) ;
	void redo();
	void undo() ;
	int id() const { return spec::exchangeDataId  ; }
};

#endif // SPECEXCHANGEDATACOMMAND_H
