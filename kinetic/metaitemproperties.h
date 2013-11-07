#ifndef METAITEMPROPERTIES_H
#define METAITEMPROPERTIES_H

#include <QDialog>
#include "specmetaitem.h"
#include <QMap>

class QListWidgetItem ;
class QTableWidgetItem ;

namespace Ui
{
	class metaItemProperties;
}

class metaItemProperties : public QDialog
{
	Q_OBJECT

public:
	explicit metaItemProperties(specMetaItem*, QWidget* parent = 0);
	~metaItemProperties();
	specUndoCommand* changedConnections(QObject* parent) ;
private slots:
	void on_connectedItemsList_itemSelectionChanged();
	void on_moveUpButton_clicked();
	void on_moveDownButton_clicked();
	void on_dataTable_itemSelectionChanged();

private:
	Ui::metaItemProperties* ui;
	specMetaItem* originalItem ;
	void moveSelection(bool) ;
	QwtPlotCurve metaCurve, currentCurve, selectedCurve ;

	struct itemLink
	{
		specModelItem* item ;
		QVector<QTableWidgetItem*> points ;
		QwtPlotCurve* curve ;
	};
	QMap<QListWidgetItem*, itemLink> itemInfo ;

	struct pointLink
	{
		QVector<QListWidgetItem*> items ;
		QPointF value ;
	};
	QMap<QTableWidgetItem*, pointLink> pointInfo ;

	QMap<specModelItem*, QListWidgetItem*> modelItemInfo ;

	void buildAssignments(const QList<specModelItem*>& items) ;
	void buildPoints() ;
	void refreshPlots() ;
	void clearItemInfo() ;
	QTableWidgetItem* firstEntry(QTableWidgetItem*) ;
	bool reselecting ;

	void checkSelection(Qt::CheckState) ;
private slots:

	void on_connectedItemsList_itemChanged(QListWidgetItem* item);
	void on_removeSelectedConnections_clicked();
	void on_addSelectedConnections_clicked();
};

#endif // METAITEMPROPERTIES_H
