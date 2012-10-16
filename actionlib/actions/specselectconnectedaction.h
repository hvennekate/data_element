#ifndef SPECSELECTCONNECTEDACTION_H
#define SPECSELECTCONNECTEDACTION_H

#include "specitemaction.h"

class specSelectConnectedAction : public specItemAction
{
	Q_OBJECT
private:
	bool requirements() ;
	specUndoCommand* generateUndoCommand() ;
public:
	explicit specSelectConnectedAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
};

#endif // SPECSELECTCONNECTEDACTION_H
