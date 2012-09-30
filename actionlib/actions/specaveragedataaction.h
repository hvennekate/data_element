#ifndef SPECAVERAGEDATAACTION_H
#define SPECAVERAGEDATAACTION_H

#include "specrequiresitemaction.h"

class specAverageDataAction : public specRequiresItemAction
{
    Q_OBJECT
public:
	explicit specAverageDataAction(QObject *parent = 0);
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECAVERAGEDATAACTION_H
