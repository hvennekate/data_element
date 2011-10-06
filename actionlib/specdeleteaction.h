#ifndef SPECDELETEACTION_H
#define SPECDELETEACTION_H

#include "specundoaction.h"
#include "specdataview.h"

class specDeleteAction : public specUndoAction
{
    Q_OBJECT
public:
	explicit specDeleteAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;

signals:

public slots:
protected:
	void execute() ;

};

#endif // SPECDELETEACTION_H
