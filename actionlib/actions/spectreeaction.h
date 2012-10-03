#ifndef SPECTREEACTION_H
#define SPECTREEACTION_H

#include "specitemaction.h"

class specTreeAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specTreeAction(QObject *parent = 0);
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECTREEACTION_H
