#ifndef SPECSPECTRUMCALCULATORACTION_H
#define SPECSPECTRUMCALCULATORACTION_H
#include "specrequiresitemaction.h"

class spectrumCalculatorDialog ;

class specSpectrumCalculatorAction : public specRequiresItemAction
{
	Q_OBJECT
private:
	spectrumCalculatorDialog* dialog ;
public:
	explicit specSpectrumCalculatorAction(QObject *parent = 0);
	~specSpectrumCalculatorAction() ;
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
public:
	static QStringList descriptorNames(QString &expression) ;
	static QString parameterName(int i) ;
};

#endif // SPECSPECTRUMCALCULATORACTION_H
