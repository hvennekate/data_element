#include "specmetafilter.h"


specMetaFilter::specMetaFilter(specMetaItem *par)
{
	setParent(par) ;
}

void specMetaFilter::setParent(specMetaItem *par)
{
	if (parent)
		parent->setFilter(0) ;
	parent = par ;
	if (parent)
		parent->setFilter(this) ;
}


