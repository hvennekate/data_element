#ifndef SPECREMOVEFITACTION_H
#define SPECREMOVEFITACTION_H

#include "specrequiresitemaction.h"

class specRemoveFitAction : public specRequiresMetaItemAction
{
	Q_OBJECT
public:
	explicit specRemoveFitAction(QObject* parent = 0);

private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECREMOVEFITACTION_H
