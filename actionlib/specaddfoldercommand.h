#ifndef SPECADDFOLDERCOMMAND_H
#define SPECADDFOLDERCOMMAND_H

#include "specmanageitemscommand.h"
#include "specmodel.h"
#include "specgenealogy.h"

class specAddFolderCommand : public specManageItemsCommand
{
public:
	specAddFolderCommand(specUndoCommand *parent = 0);
	void redo() ;
	void undo() ;
};

#endif // SPECADDFOLDERCOMMAND_H
