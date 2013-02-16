#ifndef SPECIMPORTSPECACTION_H
#define SPECIMPORTSPECACTION_H
#include "specitemaction.h"

class QFile ;
class specModelItem ;

class specImportSpecAction : public specItemAction
{
	Q_OBJECT
	QStringList filters ;
public:
	explicit specImportSpecAction(QObject *parent = 0);
	void setFilters(const QStringList& f) ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECIMPORTSPECACTION_H
