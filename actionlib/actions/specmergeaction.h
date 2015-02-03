#ifndef SPECMERGEACTION_H
#define SPECMERGEACTION_H

#include "specrequiresitemaction.h"
#include "specdataview.h"
#include "specdescriptorcomparisoncriterion.h"
#include "specworkerthread.h"

class specMergeDialog ;
class specDataItem ;

class specMergeAction : public specRequiresDataItemAction
{
	Q_OBJECT
private:
	QModelIndexList allChildren(const QModelIndex& parent) const ;
public:
	explicit specMergeAction(QObject* parent = 0);
	~specMergeAction() ;
	const std::type_info& possibleParent() { return typeid(specDataView) ; }
private:
	specUndoCommand* generateUndoCommand() ;
	specMergeDialog* dialog ;

};

#endif // SPECMERGEACTION_H
