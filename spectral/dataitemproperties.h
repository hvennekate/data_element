#ifndef DATAITEMPROPERTIES_H
#define DATAITEMPROPERTIES_H

#include <QDialog>
#include "specdataitem.h"

namespace Ui
{
	class dataItemProperties;
}

class specDelegate ;

class dataItemProperties : public QDialog
{
	Q_OBJECT

public:
	dataItemProperties(specDataItem*, QWidget* parent = 0);
	~dataItemProperties();
	specUndoCommand* changeCommands(QObject*) ;
private:
	Ui::dataItemProperties* ui;
	specDataItem item ;
	specDataItem* originalItem ;
	QwtPlotCurve highlightSelected, highlightCurrent ;
	bool setupComplete ;
	bool merge, sort ;
	bool buildingDescriptorTable ;
	specDelegate *descriptorDelegate ;
	void connectCurve(QwtPlotCurve& c) ;
	void getDescriptors() ;
	QIcon numericIcon(const specDescriptor& d) ;
	QStringList tableDescriptors() const ;
	int addDescriptor(const QString& name, const QString& content, bool multiline) ;
private slots:
	void refreshPlot() ;
	void on_descriptorTable_cellChanged(int row, int column);
	void on_removeDescriptorButton_clicked();
	void on_addDescriptorButton_clicked();
};

#endif // DATAITEMPROPERTIES_H
