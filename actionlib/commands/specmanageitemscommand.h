#ifndef SPECMANAGEITEMSCOMMAND_H
#define SPECMANAGEITEMSCOMMAND_H

#include "specmultipleitemcommand.h"

class specManageItemsCommand : public specMultipleItemCommand
{
public:
	explicit specManageItemsCommand(specUndoCommand *parent = 0);
	void setItems(QList<specModelItem*>&) ;
	void setItem(specModelItem* pointer) ;
};

#endif // SPECDELETECOMMAND_H
