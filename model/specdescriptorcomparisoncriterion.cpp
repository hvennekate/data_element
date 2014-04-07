#include "specdescriptorcomparisoncriterion.h"
#include <cmath>
#include "specmodelitem.h"

specDescriptorComparisonCriterion::specDescriptorComparisonCriterion()
{
	setNumeric(false) ;
}

specDescriptorComparisonCriterion::specDescriptorComparisonCriterion(const QString &descriptor, bool numeric)
{
	setDescriptor(descriptor);
	setNumeric(numeric) ;
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

bool specDescriptorComparisonCriterion::isNumeric() const
{
	return !isnan(toleranceValue) ;
}

void specDescriptorComparisonCriterion::setNumeric(bool a)
{
	if (a != isNumeric())
		toleranceValue = a ? 0 : NAN ;
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

	if(isNumeric())
	{
		double a = first->descriptorValue(descriptorName),
		       b = second->descriptorValue(descriptorName) ;
		if(b - toleranceValue <= a && a <= b + toleranceValue)
			return true ;
		if (isnan(a) && isnan(b)) return true ; // TODO check if both have this descriptor
		return false ;
	}

	return first->descriptor(descriptorName, true)
			== second->descriptor(descriptorName, true) ;
}

bool specDescriptorComparisonCriterion::itemsEqual(specModelItem *a, specModelItem *b, const container &criteria)
{
	foreach(const specDescriptorComparisonCriterion& criterion, criteria)
		if (!criterion.itemsEqual(a,b))
			return false ;
	return true ;
}
