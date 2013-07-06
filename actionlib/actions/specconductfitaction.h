#ifndef SPECCONDUCTFITACTION_H
#define SPECCONDUCTFITACTION_H

#include "specrequiresitemaction.h"

class specConductFitAction : public specRequiresMetaItemAction
{
	Q_OBJECT
public:
	explicit specConductFitAction(QObject *parent = 0);
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECCONDUCTFITACTION_H
