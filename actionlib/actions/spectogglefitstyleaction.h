#ifndef SPECTOGGLEFITSTYLEACTION_H
#define SPECTOGGLEFITSTYLEACTION_H

#include "specitemaction.h"

class specToggleFitStyleAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specToggleFitStyleAction(QObject *parent = 0);
private:
	specUndoCommand * generateUndoCommand() ;
};

#endif // SPECTOGGLEFITSTYLEACTION_H
