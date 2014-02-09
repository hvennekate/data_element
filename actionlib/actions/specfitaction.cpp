#include "specfitaction.h"
#include "specmetaitem.h"

specFitAction::specFitAction(QObject *parent) :
	specRequiresMetaItemAction(parent)
{
}

bool specFitAction::negateFitRequirement() const
{
	return false ;
}

bool specFitAction::specificRequirements()
{
	assembleSelection();
	foreach(specModelItem* p, pointers)
	{
		specMetaItem *mi = dynamic_cast<specMetaItem*>(p) ;
		if (!p) continue ; // should not be necessary...
		bool hasFitCurve = mi->hasFitCurve() ;
		if ((hasFitCurve || negateFitRequirement())
				&& !(hasFitCurve && negateFitRequirement())) return true ;
	}
	return false ;
}
