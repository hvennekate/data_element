#ifndef SPECTOGGLEFITSTYLECOMMAND_H
#define SPECTOGGLEFITSTYLECOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"
#include "specmetaitem.h"

class specToggleFitStyleCommand : public specUndoCommand
{
private:
	specGenealogy item ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	void doIt() ;
	void undoIt() ;
	void parentAssigned();
	type typeId() const { return specStreamable::toggleFitStyleCommand ; }
public:
	explicit specToggleFitStyleCommand(specUndoCommand *parent = 0);
	void setup(const QModelIndex&) ;
};

#endif // SPECTOGGLEFITSTYLECOMMAND_H
