#ifndef SPECNEWMETAITEMACTION_H
#define SPECNEWMETAITEMACTION_H

#include "specitemaction.h"

class specNewMetaItemAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specNewMetaItemAction(QObject* parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECNEWMETAITEMACTION_H
