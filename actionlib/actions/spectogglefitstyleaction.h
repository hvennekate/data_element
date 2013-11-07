#ifndef SPECTOGGLEFITSTYLEACTION_H
#define SPECTOGGLEFITSTYLEACTION_H

#include "specrequiresitemaction.h"

class specToggleFitStyleAction : public specRequiresMetaItemAction
{
	Q_OBJECT
public:
	explicit specToggleFitStyleAction(QObject* parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECTOGGLEFITSTYLEACTION_H
