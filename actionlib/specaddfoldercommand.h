#ifndef SPECADDFOLDERCOMMAND_H
#define SPECADDFOLDERCOMMAND_H

#include "specmanageitemscommand.h"
#include "specmodel.h"
#include "specgenealogy.h"
#include "names.h"

class specAddFolderCommand : public specManageItemsCommand
{
public:
	specAddFolderCommand(specUndoCommand *parent = 0);
	void redo() ;
	void undo() ;
	int id() const { return spec::newFolderCommandId ;}
};

#endif // SPECADDFOLDERCOMMAND_H
