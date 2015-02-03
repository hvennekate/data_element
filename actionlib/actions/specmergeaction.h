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

class mergeActionThread : public specWorkerThread
{
private:
	specDescriptorComparisonCriterion::container criteria ;
	spec::correctionMode spectralAdaptation ;
	QList<specModelItem*> items ;
	QList<specModelItem*> toBeDeleted ;
	QList<specModelItem*> toInsert ;

	bool cleanUp() ;
	void mergeItems(specDataItem* newItem, const specDataItem* other) const ;

public:
	mergeActionThread(const QList<specModelItem*>& itms,
			  const specDescriptorComparisonCriterion::container& crit,
			  spec::correctionMode sadap) ;
	~mergeActionThread() ;

	QList<specModelItem*> getItemsToDelete() ;
	QList<specModelItem*> getItemsToInsert() ;

	void run() ;
};

#endif // SPECMERGEACTION_H
