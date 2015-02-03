#include "bmk_mergeitems.h"
#include "../spectral/specdataitem.h"
#include "../model/specdescriptorcomparisoncriterion.h"
#include "../actionlib/actions/specmergeaction.h"
#include "specmergerunnable.h"

#include "specmergerunner.h"
#include <QThreadPool>

Q_DECLARE_METATYPE(specDescriptorComparisonCriterion::container)

bmk_mergeItems::bmk_mergeItems(QObject *parent)
	: QObject(parent)

{
	setObjectName("Merge Benchmark") ;

}

bmk_mergeItems::~bmk_mergeItems()
{
}

void bmk_mergeItems::init()
{
	specDataItem::dataContainer data(32) ;
	QHash<QString, specDescriptor> description ;

	for (int i = 0 ; i < 20 ; ++i)
	{
		for (int k = 0 ; k < 5 ; ++k)
		{
			for (int j = 0 ; j < 100 ; ++j)
			{
				description["A"] = specDescriptor(i) ;
				description["B"] = specDescriptor(j) ;
				items << new specDataItem(data, description) ;
			}
		}
	}
}

void bmk_mergeItems::cleanup()
{
	foreach(specModelItem* item, items)
		delete item ;
	items.clear();
}

void bmk_mergeItems::merge()
{
	QFETCH(specDescriptorComparisonCriterion::container, Comparisons) ;
	QFETCH(int, ExpectedCount) ;

	mergeActionThread thread(items, Comparisons, spec::noCorrection) ;
	QBENCHMARK {
		thread.run();
	}
	QVERIFY(ExpectedCount == thread.getItemsToInsert().count());
}

void bmk_mergeItems::merge_data()
{
	QTest::addColumn<specDescriptorComparisonCriterion::container>("Comparisons") ;
	QTest::addColumn<int>("ExpectedCount") ;

	QTest::newRow("Just merge") << (specDescriptorComparisonCriterion::container()) << 1;
	QTest::newRow("First must match")
			<< (specDescriptorComparisonCriterion::container()
			    << specDescriptorComparisonCriterion("A"))
			<< 20 ;
	QTest::newRow("Second must match")
			<< (specDescriptorComparisonCriterion::container()
			    << specDescriptorComparisonCriterion("B"))
			<< 100 ;
	QTest::newRow("Both must match")
			<< (specDescriptorComparisonCriterion::container()
			    << specDescriptorComparisonCriterion("A")
			    << specDescriptorComparisonCriterion("B"))
			<< 2000 ;
	QTest::newRow("Both match, tolerance second")
			<< (specDescriptorComparisonCriterion::container()
			    << specDescriptorComparisonCriterion("A")
			    << specDescriptorComparisonCriterion("B", 3))
			<< 500 ;
}

void bmk_mergeItems::runnerMerge()
{
	QFETCH(specDescriptorComparisonCriterion::container, Comparisons) ;
	QFETCH(int, ExpectedCount) ;

	specMergeRunner::specMergeRunnerData data ;
	data.corrMode = spec::noCorrection ;
	data.criteria = Comparisons ;
	data.items = items ;
	specMergeRunner runner(data) ;
	QBENCHMARK {
		runner.run() ;
	}
	QVERIFY(ExpectedCount == runner.getItemsToInsert().count());
}

void bmk_mergeItems::runnerMerge_data()
{
	merge_data();
}

void bmk_mergeItems::parallelRunnerMerge()
{
	QFETCH(specDescriptorComparisonCriterion::container, Comparisons) ;
	QFETCH(int, ExpectedCount) ;

	QThreadPool pool ;
	QVector<specModelItem*> input(items.toVector()),
			output(items.size()) ;
	specMergeRunnable::dataContainer *originalData = new specMergeRunnable::dataContainer;
	originalData->input = &input ;
	originalData->output = &output ;
	originalData->criteria = &Comparisons ;
	originalData->criterionIndex = 0 ;
	originalData->threadPool = &pool ;
	originalData->cmode = spec::noCorrection ;
	originalData->itemIndexes = QVector<int>(items.size()) ;
	for (int i = 0 ; i < items.size() ; ++i)
		originalData->itemIndexes[i] = i ;
	specMergeRunnable *runnable = new specMergeRunnable(originalData) ;
	QBENCHMARK_ONCE { // TODO plain QBENCHMARK crashes
		pool.start(runnable) ;
		while(pool.activeThreadCount()) ;
	}
	output.removeAll(0) ;

	QVERIFY(ExpectedCount == output.size()) ;
	qDebug() << "Terminated parallel execution" ;
}

void bmk_mergeItems::parallelRunnerMerge_data()
{
	merge_data() ;
}

