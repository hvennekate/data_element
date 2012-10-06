#include "specrequiresitemaction.h"

specRequiresItemAction::specRequiresItemAction(QObject *parent) :
    specItemAction(parent)
{
}

bool specRequiresItemAction::requirements()
{
	return !selection.isEmpty() ;
}
