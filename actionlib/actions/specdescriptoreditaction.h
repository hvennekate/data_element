#ifndef SPECDESCRIPTOREDITACTION_H
#define SPECDESCRIPTOREDITACTION_H

#include "specundoaction.h"

class specDescriptorEditAction : public specUndoAction
{
    Q_OBJECT
public:
    explicit specDescriptorEditAction(QObject* parent = 0) ;
private:
    specUndoCommand* generateUndoCommand() ;
};

#endif // STRINGLISTCHANGEDIALOG_H
