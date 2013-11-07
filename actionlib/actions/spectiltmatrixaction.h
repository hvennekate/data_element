#ifndef SPECTILTMATRIXACTION_H
#define SPECTILTMATRIXACTION_H

#include "specrequiresitemaction.h"

class specExchangeDescriptorXDialog ;

class specTiltMatrixAction : public specRequiresDataItemAction
{
	Q_OBJECT
public:
	explicit specTiltMatrixAction(QObject* parent = 0);
	~specTiltMatrixAction() ;
protected:
	specUndoCommand* generateUndoCommand() ;
private:
	specExchangeDescriptorXDialog* dialog ;
};

#endif // SPECTILTMATRIXACTION_H
