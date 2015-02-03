#include <QThreadPool>
#include "specmergerunnable.h"
#include "specmergerunner.h"
#include "specdataitem.h"

specMergeRunnable::specMergeRunnable(dataContainer *data)
	: d(data)
{
}

specMergeRunnable::~specMergeRunnable()
{
	delete d ;
}

void specMergeRunnable::run()
{
	if (!d || !d->valid()) return ;

	// if no criteria left -> merge!
	if (d->criteria->size() == d->criterionIndex)
	{
		QList<specModelItem*> itemList ;
		foreach(const int& i, d->itemIndexes)
			itemList << d->input->at(i) ;
		d->output->operator [](d->itemIndexes.first())
				= mergeItems(itemList, d->cmode) ;
		return ;
	}

	// otherwise: create new runnables from criterion
	typedef QPair<specDescriptor, QVector<int> > descriptorItemListPair ;
	QList<descriptorItemListPair> groups ;
	specDescriptorComparisonCriterion criterion(d->criteria->at(d->criterionIndex)) ;
	double tolerance = criterion.tolerance() ;
	foreach(const int& itemIndex, d->itemIndexes)
	{
		// sort out non-dataItems
		specDataItem* item = dynamic_cast<specDataItem*>(d->input->at(itemIndex)) ;
		if (!item)
		{
			d->input->operator[](itemIndex) = 0 ;
			continue ;
		}
		// assign group to others
		specDescriptor descriptor = item->descriptor(criterion.descriptor()) ;
		int i = 0 ;
		for (i = 0 ; i < groups.size() ; ++i)
		{
			if (descriptor.fuzzyComparison(groups[i].first, tolerance))
			{
				groups[i].second << itemIndex ;
				break ;
			}
		}
		if (i == groups.size())
			groups << qMakePair(descriptor, QVector<int>() << itemIndex) ;
	}

	foreach(const descriptorItemListPair& group, groups)
	{
		dataContainer *newContainer = new dataContainer(*d) ;
		newContainer->itemIndexes = group.second ;
		specMergeRunnable* newRunnable = new specMergeRunnable(newContainer) ;
		d->threadPool->start(newRunnable) ;
	}
}



specMergeRunnable::dataContainer::dataContainer(const specMergeRunnable::dataContainer &other)
	: input(other.input),
	  output(other.output),
	  criteria(other.criteria),
	  criterionIndex(other.criterionIndex+1),
	  threadPool(other.threadPool),
	  cmode(other.cmode)
{
}

specMergeRunnable::dataContainer::dataContainer()
	: input(0),
	  output(0),
	  criteria(0),
	  criterionIndex(0),
	  threadPool(0),
	  cmode(spec::noCorrection)
{
}

bool specMergeRunnable::dataContainer::valid()
{
	return itemIndexes.size()
			&& threadPool
			&& input
			&& output
			&& criteria ;
}
