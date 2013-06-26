#ifndef SPECSETMULTILINEACTION_H
#define SPECSETMULTILINEACTION_H

#include "specitemaction.h"

class specSetMultilineAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specSetMultilineAction(QObject *parent = 0);
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECSETMULTILINEACTION_H
