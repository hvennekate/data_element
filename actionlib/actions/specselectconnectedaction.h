#ifndef SPECSELECTCONNECTEDACTION_H
#define SPECSELECTCONNECTEDACTION_H

#include "specconnectionsaction.h"

class specMetaView ;

class specSelectConnectedAction : public specConnectionsAction
{
	Q_OBJECT
private:
	specUndoCommand* generateUndoCommand() ;
public:
	explicit specSelectConnectedAction(QObject *parent = 0);
};

#endif // SPECSELECTCONNECTEDACTION_H
