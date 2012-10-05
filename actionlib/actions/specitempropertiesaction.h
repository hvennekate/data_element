#ifndef SPECITEMPROPERTIESACTION_H
#define SPECITEMPROPERTIESACTION_H

#include "specitemaction.h"

class specItemPropertiesAction : public specItemAction
{
    Q_OBJECT
public:
    explicit specItemPropertiesAction(QObject *parent = 0);

private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECITEMPROPERTIESACTION_H
