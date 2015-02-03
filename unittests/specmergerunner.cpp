#include "specmergerunner.h"
#include "specdataitem.h"
#include "specfiltergenerator.h"

class specMergeRunner::privateData
{
public:
	specMergeRunnerData data ;
	specModelItem* result ;
};


specMergeRunner::specMergeRunner(const specMergeRunner::specMergeRunnerData &data)
	: d(new privateData)
{
	d->data = data ;
	d->result = 0 ;
}

specDataItem* mergeItems(QList<specModelItem*>& items, spec::correctionMode cmode) // TODO QList<const specModelItem*>
{
	specDataItem *newItem = new specDataItem ;

	specFilterGenerator filterGenerator ;
	filterGenerator.setMode(cmode) ;
	foreach (specModelItem* item, items)
	{
		specDataItem* other = dynamic_cast<specDataItem*>(item) ;
		if (!other) continue ; // TODO create temp list and swap for real data items
		specDataItem* correctedItem = 0 ;
		if (filterGenerator.valid() && newItem->size())
		{
			filterGenerator.setReference(newItem->dataMap());
			correctedItem = new specDataItem(*other) ;
			specDataPointFilter filter = filterGenerator.generateFilter(correctedItem) ;
			if (filter.valid())
			{
				correctedItem->addDataFilter(filter) ;
				other = correctedItem ; // alternatively: const_cast
			}
		}

		*newItem += *other ;
		delete correctedItem ;
	}

	newItem->flatten();
	return newItem ;
}



specMergeRunner::~specMergeRunner()
{

}

QList<specModelItem *> specMergeRunner::getItemsToDelete() const
{
	return d->data.items ;
}

QList<specModelItem *> specMergeRunner::getItemsToInsert() const
{
	if (d->result) return QList<specModelItem*>() << d->result ;
	QList<specModelItem*> result ;
	foreach(specMergeRunner* subRunner,
			findChildren<specMergeRunner*>(QString(), Qt::FindDirectChildrenOnly))
	{
		subRunner->wait() ;
		result << subRunner->getItemsToInsert() ;
	}
	return result ;
}

void specMergeRunner::run()
{
	// just one item -> skip
	if (d->data.items.size() < 2)
	{
		d->data.items.clear();
		return ;
	}

	// end of recursion test:
	if (d->data.criteria.empty())
	{
		d->result = mergeItems(d->data.items, d->data.corrMode) ;
		return ;
	}

	// else group items by first descriptor
	typedef QPair<specDescriptor, QList<specModelItem*> > descriptorItemListPair ;
	QList<descriptorItemListPair> groups ;
	double tolerance = d->data.criteria.first().tolerance() ;
	QString descriptorName = d->data.criteria.first().descriptor() ;
	foreach(specModelItem* item, d->data.items)
	{
		specDescriptor descriptor = item->descriptor(descriptorName) ;
		int i = 0 ;
		for (i = 0 ; i < groups.size() ; ++i)
		{
			if (descriptor.fuzzyComparison(groups[i].first, tolerance))
			{
				groups[i].second << item ;
				break ;
			}
		}
		if (i == groups.size())
			groups << qMakePair(descriptor, QList<specModelItem*>() << item) ;
	}
//	////////////// debugging
//	QStringList descriptors ;
//	foreach (descriptorItemListPair p, groups)
//		descriptors << p.first.content() ;
//	qDebug() << descriptors ;
//	////////////// end debugging

	// prepare new sub threads
	specMergeRunnerData newData ;
	newData.corrMode = d->data.corrMode ;
	newData.criteria = d->data.criteria.mid(1) ;

	foreach(const descriptorItemListPair& group, groups)
	{
		newData.items = group.second ;
		specMergeRunner *subRunner = new specMergeRunner(newData) ;
		subRunner->setParent(this) ;
		subRunner->setObjectName(group.first.content(true)) ;
		subRunner->run();
	}
}
