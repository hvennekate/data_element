#ifndef DATAITEMPROPERTIES_H
#define DATAITEMPROPERTIES_H

#include <QDialog>
#include "specdataitem.h"

namespace Ui {
class dataItemProperties;
}

class dataItemProperties : public QDialog
{
	Q_OBJECT

public:
	dataItemProperties(specDataItem*, QWidget *parent = 0);
	~dataItemProperties();
	specUndoCommand *changeCommands(QObject*) ;
private:
	Ui::dataItemProperties *ui;
	specDataItem item ;
	specDataItem *originalItem ;
	QwtPlotCurve highlightSelected, highlightCurrent ;
	bool setupComplete ;
private slots:
	void refreshPlot() ;
};

#endif // DATAITEMPROPERTIES_H
