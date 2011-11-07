#ifndef SPECMERGEACTION_H
#define SPECMERGEACTION_H

#include "specundoaction.h"
#include "specview.h"

class specMergeAction : public specUndoAction
{
    Q_OBJECT
public:
	explicit specTreeAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specView) ; }
protected:
	void execute() ;

};

#endif // SPECMERGEACTION_H
