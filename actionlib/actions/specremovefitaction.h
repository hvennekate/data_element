#ifndef SPECREMOVEFITACTION_H
#define SPECREMOVEFITACTION_H

#include "specfitaction.h"

class specRemoveFitAction : public specFitAction
{
	Q_OBJECT
public:
	explicit specRemoveFitAction(QObject* parent = 0);

private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECREMOVEFITACTION_H
