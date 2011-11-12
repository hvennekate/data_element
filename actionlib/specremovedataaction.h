#ifndef SPECREMOVEDATAACTION_H
#define SPECREMOVEDATAACTION_H

#include "spectral/specdataview.h"
#include "specundoaction.h"

class specRemoveDataAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specRemoveDataAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specDataView) ; }
protected:
	void execute() ;
};

#endif // SPECREMOVEDATAACTION_H
