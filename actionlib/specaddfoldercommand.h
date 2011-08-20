#ifndef SPECADDFOLDERCOMMAND_H
#define SPECADDFOLDERCOMMAND_H

#include "specundocommand.h"
#include "specmodel.h"
#include "specgenealogy.h"

class specAddFolderCommand : public specUndoCommand
{
public:
	specAddFolderCommand(specUndoCommand *parent = 0);
	void setPosition(specGenealogy*) ;
	void redo() ;
	void undo() ;
	bool ok() ;
private:
	specModel* model ;
	specGenealogy* position ;

};

#endif // SPECADDFOLDERCOMMAND_H
