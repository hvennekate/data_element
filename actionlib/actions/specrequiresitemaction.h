#ifndef SPECREQUIRESITEMACTION_H
#define SPECREQUIRESITEMACTION_H
#include "specitemaction.h"

class specRequiresItemAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specRequiresItemAction(QObject *parent = 0);
private:
	bool requirements() ;
};

#endif // SPECREQUIRESITEMACTION_H
