#ifndef MATRIXEXPORTACTION_H
#define MATRIXEXPORTACTION_H
#include "specrequiresitemaction.h"

namespace Ui
{
	class matrixExportDialog ;
}
class QDialog ;

class matrixExportAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit matrixExportAction(QObject *parent = 0);
	~matrixExportAction() ;
private:
	specUndoCommand* generateUndoCommand() ;
	Ui::matrixExportDialog* dialog ;
	QDialog* d ;
};

#endif // MATRIXEXPORTACTION_H
