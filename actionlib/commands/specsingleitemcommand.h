#ifndef SPECITEMCOMMAND_H
#define SPECITEMCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

template <class itemType>
class specSingleItemCommand : public specUndoCommand
{
private:
	specGenealogy item ;
	void writeCommand(QDataStream& out) const ;
	void readCommand(QDataStream& in) ;
protected:
	void writeItem(QDataStream& out) const ;
	void readItem(QDataStream& in) ;
	void parentAssigned() ;
	itemType* itemPointer() ;
	itemType* itemPointer() const ;
public:
	specSingleItemCommand(specUndoCommand* parent = 0);
	virtual void setItem(specModelItem*) ;
};

#endif // SPECITEMCOMMAND_H
