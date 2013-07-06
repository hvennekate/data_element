#ifndef SPECSPECTRUMCALCULATORACTION_H
#define SPECSPECTRUMCALCULATORACTION_H
#include "specrequiresitemaction.h"

class specSpectrumCalculatorAction : public specRequiresDataItemAction
{
	Q_OBJECT
public:
	explicit specSpectrumCalculatorAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
public:
	static QStringList descriptorNames(QString &expression) ;
	static QString parameterName(int i) ;
};

#endif // SPECSPECTRUMCALCULATORACTION_H
