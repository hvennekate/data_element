#ifndef SPECDELETECOMMAND_H
#define SPECDELETECOMMAND_H

#include "specmodel.h"
#include "specmanageitemscommand.h"
#include <QVector>
#include <QPair>
#include <QHash>
#include "specgenealogy.h"
#include "names.h"

class specDeleteCommand : public specManageItemsCommand
{
private:
	void doIt() ;
	void undoIt() ;
	type typeId() const { return specStreamable::deleteCommandId; }
public:
	explicit specDeleteCommand(specUndoCommand *parent = 0);
};

#endif // SPECDELETECOMMAND_H
