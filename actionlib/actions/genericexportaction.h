#ifndef GENERICEXPORTACTION_H
#define GENERICEXPORTACTION_H
#include "specrequiresitemaction.h"

class exportDialog ;

class genericExportAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit genericExportAction(QObject *parent = 0);
	~genericExportAction() ;
private:
	specUndoCommand* generateUndoCommand() ;
	exportDialog *exportFormat ;
};

#endif // GENERICEXPORTACTION_H
