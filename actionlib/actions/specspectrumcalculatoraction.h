#ifndef SPECSPECTRUMCALCULATORACTION_H
#define SPECSPECTRUMCALCULATORACTION_H
#include "specrequiresitemaction.h"

class specSpectrumCalculatorAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specSpectrumCalculatorAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECSPECTRUMCALCULATORACTION_H
