#ifndef SPECMERGEACTION_H
#define SPECMERGEACTION_H

#include "specitemaction.h"
#include "specdataview.h"

class specMergeDialog ;

class specMergeAction : public specItemAction
{
	Q_OBJECT
private:
	QModelIndexList allChildren(const QModelIndex& parent) const ;
public:
	explicit specMergeAction(QObject *parent = 0);
	~specMergeAction() ;
	const std::type_info& possibleParent() { return typeid(specDataView) ; }
private:
	specUndoCommand* generateUndoCommand() ;
	specMergeDialog* dialog ;

};

#endif // SPECMERGEACTION_H
