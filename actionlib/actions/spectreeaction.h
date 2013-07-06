#ifndef SPECTREEACTION_H
#define SPECTREEACTION_H

#include "specrequiresitemaction.h"

class specTreeAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specTreeAction(QObject *parent = 0);
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECTREEACTION_H
