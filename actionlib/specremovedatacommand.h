#ifndef SPECREMOVEDATACOMMAND_H
#define SPECREMOVEDATACOMMAND_H

#include "specmanagedatacommand.h"

class specRemoveDataCommand : public specManageDataCommand
{
public:
	specRemoveDataCommand(specUndoCommand *parent = 0) ;
	void redo() ;
	void undo() ;
	int id() const { return spec::removeDataId ; }
};

#endif // SPECREMOVEDATACOMMAND_H
