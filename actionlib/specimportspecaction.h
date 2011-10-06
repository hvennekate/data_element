#ifndef SPECIMPORTSPECACTION_H
#define SPECIMPORTSPECACTION_H
#include "specundoaction.h"

class specImportSpecAction : public specUndoAction
{
    Q_OBJECT
public:
    explicit specImportSpecAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;

signals:

public slots:

private:
	void execute() ;

};

#endif // SPECIMPORTSPECACTION_H
