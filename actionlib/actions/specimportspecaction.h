#ifndef SPECIMPORTSPECACTION_H
#define SPECIMPORTSPECACTION_H
#include "specundoaction.h"

class QFile ;
class specModelItem ;

class specImportSpecAction : public specUndoAction
{
	Q_OBJECT
	QStringList filters ;
	QList<QList<specModelItem*> (*)(QFile&)> acceptableFunctions ;
public:
	explicit specImportSpecAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
	void setFilters(const QStringList& f) ;
	void setAcceptableImportFunctions(const QList<QList<specModelItem*> (*)(QFile&)>& f) ;
private:
	void execute() ;

};

#endif // SPECIMPORTSPECACTION_H
