#ifndef SPECADDCONNECTIONSACTION_H
#define SPECADDCONNECTIONSACTION_H

#include "specconnectionsaction.h"

class specAddConnectionsAction : public specConnectionsAction
{
	Q_OBJECT
public:
	explicit specAddConnectionsAction(QObject* parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
	bool dataViewRequirement, metaViewRequirement, changing ;
	void checkRequirements() ;
private slots:
	void changeByDataView() ;
	void changedByMetaView() ;
};

#endif // SPECADDCONNECTIONSACTION_H
