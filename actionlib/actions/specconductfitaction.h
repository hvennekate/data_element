#ifndef SPECCONDUCTFITACTION_H
#define SPECCONDUCTFITACTION_H

#include "specfitaction.h"

class specConductFitAction : public specFitAction
{
	Q_OBJECT
public:
	explicit specConductFitAction(QObject* parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECCONDUCTFITACTION_H
