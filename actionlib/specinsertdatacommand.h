#ifndef SPECINSERTDATACOMMAND_H
#define SPECINSERTDATACOMMAND_H

#include "specmanagedatacommand.h"

class specInsertDataCommand : public specManageDataCommand
{
public:
	specInsertDataCommand(specUndoCommand* parent = 0);
	void redo() ;
	void undo() ;
	int id() const { return spec::insertDataId ; }
};

#endif // SPECINSERTDATACOMMAND_H
