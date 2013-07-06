#ifndef SPECNORMALIZEACTION_H
#define SPECNORMALIZEACTION_H

#include "specrequiresitemaction.h"

class QDialog ;
namespace  Ui {
	class specNormalizeActionDialog ;
}

class specNormalizeAction : public specRequiresDataItemAction
{
	Q_OBJECT
public:
	explicit specNormalizeAction(QObject *parent = 0);
	~specNormalizeAction() ;
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
private:
	QDialog *uiDialog ;
	Ui::specNormalizeActionDialog* ui ;
};

#endif // SPECNORMALIZEACTION_H
