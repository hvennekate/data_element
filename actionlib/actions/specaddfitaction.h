#ifndef SPECADDFITACTION_H
#define SPECADDFITACTION_H

#include "specfitaction.h"

class specAddFitAction : public specFitAction
{
	Q_OBJECT
public:
	explicit specAddFitAction(QObject* parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
	bool negateFitRequirement() const ;
};

#endif // SPECADDFITACTION_H
