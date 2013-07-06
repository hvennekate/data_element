#ifndef SPECTILTMATRIXACTION_H
#define SPECTILTMATRIXACTION_H

#include "specrequiresitemaction.h"

class specTiltMatrixAction : public specRequiresDataItemAction
{
	Q_OBJECT
public:
	explicit specTiltMatrixAction(QObject *parent = 0);
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECTILTMATRIXACTION_H
