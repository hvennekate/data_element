#ifndef SPECAVERAGEDATAACTION_H
#define SPECAVERAGEDATAACTION_H

#include "specundoaction.h"
#include "specdataview.h"

class specAverageDataAction : public specUndoAction
{
    Q_OBJECT
public:
    explicit specAverageDataAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specDataView) ; }
protected:
	void execute() ;
};

#endif // SPECAVERAGEDATAACTION_H
