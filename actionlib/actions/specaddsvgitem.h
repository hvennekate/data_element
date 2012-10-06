#ifndef SPECADDSVGITEMACTION_H
#define SPECADDSVGITEMACTION_H

#include "specitemaction.h"

class specAddSVGItemAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specAddSVGItemAction(QObject *parent = 0);
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECADDSVGITEMACTION_H
