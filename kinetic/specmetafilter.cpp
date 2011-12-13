#include "specmetafilter.h"


specMetaFilter::specMetaFilter(specMetaItem *par)
{
	setParent(par) ;
}

void specMetaFilter::setParent(specMetaItem *par)
{
	if (parent)
		parent->takeFilter() ;
	parent = par ;
	if (parent)
		parent->setFilter(this) ;
}

bool specMetaFilter::lessThanXValue(const QPointF &first, const QPointF &second)
{
	return first.x() < second.x() ;
}
