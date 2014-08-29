#ifndef SPECSETMULTILINEACTION_H
#define SPECSETMULTILINEACTION_H

#include "specrequiresitemaction.h"

class specSetMultilineAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specSetMultilineAction(QObject* parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
	bool specificCheckRequirements() ;
};

#endif // SPECSETMULTILINEACTION_H
