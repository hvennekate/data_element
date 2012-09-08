#ifndef SPECNEWMETAITEMACTION_H
#define SPECNEWMETAITEMACTION_H

#include "specundoaction.h"

class specNewMetaItemAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specNewMetaItemAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;
private:
	void execute() ;
};

#endif // SPECNEWMETAITEMACTION_H
