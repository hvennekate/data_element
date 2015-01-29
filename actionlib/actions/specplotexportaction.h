#ifndef SPECPLOTEXPORTACTION_H
#define SPECPLOTEXPORTACTION_H

#include "specabstractplotaction.h"

namespace Ui
{
	class specSizeDialog ;
}

class QDialog ;

class specPlotExportAction : public specAbstractPlotAction
{
	Q_OBJECT
public:
	explicit specPlotExportAction(QObject *parent = 0);
private:
	void execute();
	QDialog* dialog ;
	Ui::specSizeDialog* ui ;
};

#endif // SPECPLOTEXPORTACTION_H
