#ifndef SPECADDFITACTION_H
#define SPECADDFITACTION_H

#include "specitemaction.h"

class specAddFitAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specAddFitAction(QObject *parent = 0);
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECADDFITACTION_H
