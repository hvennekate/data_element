#include "specdescriptorcomparisoncriterion.h"
#include <cmath>
#include "specmodelitem.h"

specDescriptorComparisonCriterion::specDescriptorComparisonCriterion()
{
	setTolerance(0);
}

specDescriptorComparisonCriterion::specDescriptorComparisonCriterion(const QString &descriptor, double tol)
	:  descriptorName(descriptor)
{
	setDescriptor(descriptor);
	setTolerance(tol);
}

QString specDescriptorComparisonCriterion::descriptor() const
{
	return descriptorName ;
}

void specDescriptorComparisonCriterion::setDescriptor(const QString &s)
{
	descriptorName = s ;
}

double specDescriptorComparisonCriterion::tolerance() const
{
	return toleranceValue ;
}

void specDescriptorComparisonCriterion::setTolerance(double d)
{
	toleranceValue = fabs(d) ;
}

bool specDescriptorComparisonCriterion::itemsEqual(const specModelItem *first, const specModelItem *second) const
{
	if(!first || !second)
		return false ;
	if (first == second) return true ; // TODO : necessary?

	return first->getDescriptor(descriptorName)
			.fuzzyComparison(second->getDescriptor(descriptorName), toleranceValue);
}

bool specDescriptorComparisonCriterion::itemMatchesDescriptor(const specModelItem *item, const specDescriptor &descriptor) const
{
	if (!item) return false ;
	return item->getDescriptor(descriptorName).fuzzyComparison(descriptor, toleranceValue) ;
}

bool specDescriptorComparisonCriterion::itemsEqual(specModelItem *a, specModelItem *b, const container &criteria)
{
	foreach(const specDescriptorComparisonCriterion& criterion, criteria)
		if (!criterion.itemsEqual(a,b))
			return false ;
	return true ;
}
