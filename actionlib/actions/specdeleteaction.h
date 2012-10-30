#ifndef SPECDELETEACTION_H
#define SPECDELETEACTION_H

#include "specrequiresitemaction.h"

class specDeleteAction : public specRequiresItemAction
{
    Q_OBJECT
public:
    explicit specDeleteAction(QObject *parent = 0);
    static specUndoCommand* command(specModel *model, QModelIndexList &selection) ;
protected:
    specUndoCommand* generateUndoCommand() ;

};

#endif // SPECDELETEACTION_H
