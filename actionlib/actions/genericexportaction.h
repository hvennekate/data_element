#ifndef GENERICEXPORTACTION_H
#define GENERICEXPORTACTION_H
#include "specrequiresitemaction.h"

class genericExportAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit genericExportAction(QObject *parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // GENERICEXPORTACTION_H
