#ifndef SPECIMPORTSPECACTION_H
#define SPECIMPORTSPECACTION_H
#include "specitemaction.h"

class QFile ;
class specModelItem ;

class specImportSpecAction : public specItemAction
{
	Q_OBJECT
	QStringList filters ;
	QList<QList<specModelItem*> (*)(QFile&)> acceptableFunctions ;
public:
	explicit specImportSpecAction(QObject *parent = 0);
	void setFilters(const QStringList& f) ;
	void setAcceptableImportFunctions(const QList<QList<specModelItem*> (*)(QFile&)>& f) ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECIMPORTSPECACTION_H
