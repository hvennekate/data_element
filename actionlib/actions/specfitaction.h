#ifndef SPECFITACTION_H
#define SPECFITACTION_H
#include "specrequiresitemaction.h"

class specFitAction : public specRequiresMetaItemAction
{
	Q_OBJECT
protected:
	virtual bool negateFitRequirement() const ;
	bool specificRequirements() ;
public:
	explicit specFitAction(QObject *parent = 0);

};

#endif // SPECFITACTION_H
