#ifndef SPECREMOVEFITACTION_H
#define SPECREMOVEFITACTION_H

#include "specitemaction.h"

class specRemoveFitAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specRemoveFitAction(QObject *parent = 0);

private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECREMOVEFITACTION_H
