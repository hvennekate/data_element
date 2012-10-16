#ifndef METAITEMPROPERTIES_H
#define METAITEMPROPERTIES_H

#include <QDialog>
#include "specmetaitem.h"

namespace Ui {
class metaItemProperties;
}

class metaItemProperties : public QDialog
{
	Q_OBJECT

public:
	explicit metaItemProperties(specMetaItem*, QWidget *parent = 0);
	~metaItemProperties();
	specUndoCommand* changedConnections(QObject* parent) ;
private slots:
	void on_connectedItemsList_itemSelectionChanged();
	void on_moveUpButton_clicked();
	void on_moveDownButton_clicked();
	void refreshMetaPlot() ;
private:
	Ui::metaItemProperties *ui;
	specMetaItem* originalItem ;
	QList<QPair<int,QwtPlotCurve*> > connectedData ;
	void moveSelection(bool) ;
	QModelIndexList generateConnectionList(const QList<specModelItem*>& items) ;
	QwtPlotCurve metaCurve, currentCurve, selectedCurve ;
	QVector<int> pointToItem ;
	bool refreshing ;
};

#endif // METAITEMPROPERTIES_H
