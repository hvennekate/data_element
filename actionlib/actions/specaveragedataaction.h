#ifndef SPECAVERAGEDATAACTION_H
#define SPECAVERAGEDATAACTION_H

#include "specrequiresitemaction.h"

class specAverageDataAction : public specRequiresDataItemAction
{
	Q_OBJECT
public:
	explicit specAverageDataAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECAVERAGEDATAACTION_H
