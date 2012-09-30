#ifndef SPECREMOVEDATAACTION_H
#define SPECREMOVEDATAACTION_H

#include "specdataview.h"
#include "specrequiresitemaction.h"

class specRemoveDataAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specRemoveDataAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECREMOVEDATAACTION_H
