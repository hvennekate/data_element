#ifndef SPECADDCONNECTIONSACTION_H
#define SPECADDCONNECTIONSACTION_H

#include "specconnectionsaction.h"

class specAddConnectionsAction : public specConnectionsAction
{
	Q_OBJECT
public:
	explicit specAddConnectionsAction(QObject *parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
	bool requirements() ;
	QModelIndexList servers ;
};

#endif // SPECADDCONNECTIONSACTION_H
