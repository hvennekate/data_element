#ifndef SPECITEMCOMMAND_H
#define SPECITEMCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specMultipleItemCommand : public specUndoCommand
{
protected:
	void writeCommand(QDataStream& out) ;
	virtual void setItems(QList<specModelItem*>&) ;
public:
	specMultipleItemCommand(specUndoCommand* parent = 0) ;
	virtual ~specMultipleItemCommand() ;
};

class specSingleItemCommand : public specUndoCommand
{
protected:
	specGenealogy item ;
	void writeCommand(QDataStream& out) ;
	virtual void setItem(specModelItem*) ;
public:
	specSingleItemCommand(specUndoCommand *parent = 0);
	virtual ~specSingleItemCommand() ;
};

#endif // SPECITEMCOMMAND_H
