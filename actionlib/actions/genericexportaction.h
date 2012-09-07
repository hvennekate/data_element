#ifndef GENERICEXPORTACTION_H
#define GENERICEXPORTACTION_H
#include "specundoaction.h"

class genericExportAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit genericExportAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;
private:
	void execute() ;
};

#endif // GENERICEXPORTACTION_H
