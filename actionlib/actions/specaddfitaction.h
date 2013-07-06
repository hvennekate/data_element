#ifndef SPECADDFITACTION_H
#define SPECADDFITACTION_H

#include "specrequiresitemaction.h"

class specAddFitAction : public specRequiresMetaItemAction
{
	Q_OBJECT
public:
	explicit specAddFitAction(QObject *parent = 0);
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECADDFITACTION_H
