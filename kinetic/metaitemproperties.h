#ifndef METAITEMPROPERTIES_H
#define METAITEMPROPERTIES_H

#include <QDialog>
#include "specmetaitem.h"
#include <QMap>

class QListWidgetItem ;
class QTableWidgetItem ;

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
    void on_dataTable_itemSelectionChanged();

private:
	Ui::metaItemProperties *ui;
	specMetaItem* originalItem ;
	void moveSelection(bool) ;
    QwtPlotCurve metaCurve, currentCurve, selectedCurve ;

    struct itemLink
    {
        specModelItem* item ;
        QVector<QTableWidgetItem*> points ;
        QwtPlotCurve* curve ;
        ~itemLink() { delete curve ; }
    };
    QMap<QListWidgetItem*,itemLink> itemInfo ;

    struct pointLink
    {
        QVector<QListWidgetItem*> items ;
        QPointF value ;
    };
    QMap<QTableWidgetItem*, pointLink> pointInfo ;

    void buildAssignments(const QList<QPair<specModelItem *, bool> > &items) ;
    void buildPoints() ;
    void refreshPlots() ;
    QTableWidgetItem* firstEntry(QTableWidgetItem*) ;
    bool reselecting ;
private slots:

};

#endif // METAITEMPROPERTIES_H
