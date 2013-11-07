#ifndef SPECADDFOLDERCOMMAND_H
#define SPECADDFOLDERCOMMAND_H

#include "specmanageitemscommand.h"
#include "specmodel.h"
#include "specgenealogy.h"
#include "names.h"

class specAddFolderCommand : public specManageItemsCommand
{
private:
	void doIt() ;
	void undoIt() ;
	type typeId() const { return specStreamable::newFolderCommandId ;}
public:
	specAddFolderCommand(specUndoCommand* parent = 0);
};

#endif // SPECADDFOLDERCOMMAND_H
