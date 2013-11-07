#ifndef SPECDELETEACTION_H
#define SPECDELETEACTION_H

#include "specrequiresitemaction.h"

class specDeleteAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specDeleteAction(QObject* parent = 0);
	static specUndoCommand* command(specModel* model, QList<specModelItem*>& selection, specUndoCommand* parentsParent = 0) ;
protected:
	specUndoCommand* generateUndoCommand() ;

};

#endif // SPECDELETEACTION_H
