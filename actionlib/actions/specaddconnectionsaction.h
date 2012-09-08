#ifndef SPECADDCONNECTIONSACTION_H
#define SPECADDCONNECTIONSACTION_H

#include "specundoaction.h"

class specAddConnectionsAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specAddConnectionsAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;
private:
	void execute() ;
};

#endif // SPECADDCONNECTIONSACTION_H
