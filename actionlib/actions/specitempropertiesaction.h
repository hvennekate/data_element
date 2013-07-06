#ifndef SPECITEMPROPERTIESACTION_H
#define SPECITEMPROPERTIESACTION_H

#include "specrequiresitemaction.h"

class specItemPropertiesAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specItemPropertiesAction(QObject *parent = 0);

private:
	specUndoCommand* generateUndoCommand() ;
	bool requirements() ;
};

#endif // SPECITEMPROPERTIESACTION_H
