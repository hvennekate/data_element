#ifndef SPECTOGGLEFITSTYLEACTION_H
#define SPECTOGGLEFITSTYLEACTION_H

#include "specfitaction.h"

class specToggleFitStyleAction : public specFitAction
{
	Q_OBJECT
public:
	explicit specToggleFitStyleAction(QObject* parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECTOGGLEFITSTYLEACTION_H
