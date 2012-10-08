#ifndef SPECCONDUCTFITACTION_H
#define SPECCONDUCTFITACTION_H

#include "specitemaction.h"

class specConductFitAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specConductFitAction(QObject *parent = 0);
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECCONDUCTFITACTION_H
