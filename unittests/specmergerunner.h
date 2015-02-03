#ifndef SPECMERGERUNNER_H
#define SPECMERGERUNNER_H

#include <QThread>
#include "specdescriptorcomparisoncriterion.h"
#include "names.h"

class specDataItem ;

class specMergeRunner : public QThread
{
public:
	struct specMergeRunnerData
	{
		QList<specModelItem*> items;
		specDescriptorComparisonCriterion::container criteria ;
		spec::correctionMode corrMode ;
	};

	specMergeRunner(const specMergeRunnerData& data);
	~specMergeRunner();

	QList<specModelItem*> getItemsToDelete() const ;
	QList<specModelItem*> getItemsToInsert() const ;

	void run() ;
private:
	class privateData ;
	privateData *d ;
};

specDataItem* mergeItems(QList<specModelItem*>& items, spec::correctionMode cmode) ;

#endif // SPECMERGERUNNER_H
