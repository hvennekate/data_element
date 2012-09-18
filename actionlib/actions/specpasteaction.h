#ifndef SPECPASTEACTION_H
#define SPECPASTEACTION_H

#include "specitemaction.h"

class specPasteAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specPasteAction(QObject *parent = 0);
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECPASTEACTION_H
