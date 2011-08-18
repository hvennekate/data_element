#ifndef SPECADDFOLDERCOMMAND_H
#define SPECADDFOLDERCOMMAND_H

#include "specundocommand.h"
#include "specmodel.h"

class specAddFolderCommand : public specUndoCommand
{
public:
	specAddFolderCommand(specUndoCommand *parent = 0);

	void setModel(specModel*) ;
	void redo() ;
	void undo() ;
	bool ok() ;
private:
	specModel* model ;

};

#endif // SPECADDFOLDERCOMMAND_H
