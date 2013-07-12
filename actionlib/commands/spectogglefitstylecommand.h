#ifndef SPECTOGGLEFITSTYLECOMMAND_H
#define SPECTOGGLEFITSTYLECOMMAND_H

#include "specsingleitemcommand.h"
#include "specmetaitem.h"

class specToggleFitStyleCommand : public specSingleItemCommand<specMetaItem>
{
private:
	void doIt() ;
	void undoIt() ;
	type typeId() const { return specStreamable::toggleFitStyleCommand ; }
public:
	explicit specToggleFitStyleCommand(specUndoCommand *parent = 0);
};

#endif // SPECTOGGLEFITSTYLECOMMAND_H
