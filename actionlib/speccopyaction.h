#ifndef SPECCOPYACTION_H
#define SPECCOPYACTION_H

#include "specundoaction.h"

class specCopyAction : public specUndoAction
{
    Q_OBJECT
public:
	explicit specCopyAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;

private:
	void execute() ;


};

#endif // SPECCOPYACTION_H
