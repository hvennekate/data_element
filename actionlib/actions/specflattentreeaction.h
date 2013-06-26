#ifndef SPECFLATTENTREEACTION_H
#define SPECFLATTENTREEACTION_H

#include "specrequiresitemaction.h"

class specFlattenTreeAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specFlattenTreeAction(QObject *parent = 0);
protected:
	specUndoCommand* generateUndoCommand() ;
	bool requirements() ;
};

#endif // SPECFLATTENTREEACTION_H
