#ifndef SPECDELETECOMMAND_H
#define SPECDELETECOMMAND_H

#include "specmodel.h"
#include "specmanageitemscommand.h"
#include <QVector>
#include <QPair>
#include <QHash>
#include "specgenealogy.h"

class specDeleteCommand : public specManageItemsCommand
{
public:
	explicit specDeleteCommand(specUndoCommand *parent = 0);
	void redo() ;
	void undo() ;
};

#endif // SPECDELETECOMMAND_H
