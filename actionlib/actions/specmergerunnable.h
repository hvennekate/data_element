#ifndef SPECMERGERUNNABLE_H
#define SPECMERGERUNNABLE_H


#include <QRunnable>
#include <QVector>
#include "names.h"
#include "specdescriptorcomparisoncriterion.h"

class specModelItem ;
class QThreadPool ;

class specMergeRunnable : public QRunnable
{
public:
	struct dataContainer
	{
		QVector<specModelItem*>* input ;
		QVector<specModelItem*>* output;
		const specDescriptorComparisonCriterion::container* criteria ;
		int criterionIndex ;
		QVector<int> itemIndexes ;
		QThreadPool* threadPool ;
		spec::correctionMode cmode ;

		dataContainer(const dataContainer& other) ;
		dataContainer() ;
		bool valid() ;
	};

	specMergeRunnable(dataContainer* data);
	~specMergeRunnable();
	void run() ;
private:
	dataContainer *d ;
};

#endif // SPECMERGERUNNABLE_H
