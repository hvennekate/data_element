#ifndef SPECAVERAGEDATAACTION_H
#define SPECAVERAGEDATAACTION_H

#include "specrequiresitemaction.h"
class QDialog ;

namespace Ui {
	class averageDialog ;
}

class specAverageDataAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specAverageDataAction(QObject *parent = 0);
	~specAverageDataAction() ;
	const std::type_info& possibleParent() ;
protected:
	specUndoCommand* generateUndoCommand() ;
private:
	QDialog* dialog ;
	Ui::averageDialog *ui ;
private slots:
	void runningToggled() ;
};

#endif // SPECAVERAGEDATAACTION_H
