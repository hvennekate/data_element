#ifndef SPECAVERAGENEIGHBORACTION_H
#define SPECAVERAGENEIGHBORACTION_H

#include "specrequiresitemaction.h"

class specAverageNeighborAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specAverageNeighborAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECAVERAGENEIGHBORACTION_H
