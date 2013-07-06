#ifndef SPECREMOVEDATAACTION_H
#define SPECREMOVEDATAACTION_H

#include "specrequiresitemaction.h"

class specRemoveDataAction : public specRequiresDataItemAction
{
	Q_OBJECT
public:
	explicit specRemoveDataAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECREMOVEDATAACTION_H
