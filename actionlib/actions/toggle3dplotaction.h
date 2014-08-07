#ifndef TOGGLE3DPLOTACTION_H
#define TOGGLE3DPLOTACTION_H

#include "specundoaction.h"

class QComboBox ;
class spec3dQuickPlot ;

class toggle3DPlotAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit toggle3DPlotAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
	~toggle3DPlotAction() ;
private:
	void execute() ;
	QWidget *plotWidget ;
	QComboBox *descriptors ;
	spec3dQuickPlot *quickplot ;
private slots:
	void plotWidgetDeleted() ;
};

#endif // TOGGLE3DPLOTACTION_H
