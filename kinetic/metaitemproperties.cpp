#include "metaitemproperties.h"
#include "ui_metaitemproperties.h"
#include "specaddconnectionscommand.h"
#include "specdeleteconnectionscommand.h"
#include "specmulticommand.h"
#include "spectogglefitstylecommand.h"
#include <QTableWidgetItem>

metaItemProperties::metaItemProperties(specMetaItem* i,QWidget *parent) :
	QDialog(parent),
	ui(new Ui::metaItemProperties),
	originalItem(i),
    reselecting(false)
{
	ui->setupUi(this);
	ui->moveUpButton->setIcon(QIcon::fromTheme("go-up"));
	ui->moveDownButton->setIcon(QIcon::fromTheme("go-down")) ;
    ui->addSelectedConnections->setIcon(QIcon(":/toKinetic.png")) ;
    ui->removeSelectedConnections->setIcon(QIcon::fromTheme("edit-delete")) ;

	ui->itemPreview->setAutoDelete(false) ;
    ui->styleFit->setDisabled(!i->getFitCurve()) ;
	ui->styleFit->setCheckState(i->styleFitCurve ? Qt::Checked : Qt::Unchecked) ;

	// prepare preview of meta data
	metaCurve.attach(ui->metaPlot) ;
	metaCurve.setData(i->filter->evaluate(i->items.toVector())) ;

	selectedCurve.setStyle(QwtPlotCurve::NoCurve);
	selectedCurve.setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::black), QPen(Qt::blue), QSize(5,5)));
	selectedCurve.attach(ui->metaPlot) ;

	currentCurve.setStyle(QwtPlotCurve::NoCurve);
	currentCurve.setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::red), QPen(Qt::red), QSize(5,5)));
	currentCurve.attach(ui->metaPlot) ;

    //create list and table
    buildAssignments(originalItem->serverList());

}

void metaItemProperties::refreshPlots()
{
    // item plot
    ui->itemPreview->detachItems(QwtPlotItem::Rtti_PlotItem, false) ;
    foreach(QListWidgetItem* item, ui->connectedItemsList->selectedItems())
        itemInfo[item].curve->attach(ui->itemPreview) ;

    // point plot
    QVector<QPointF> selectedPoints ;
    QSet<QTableWidgetItem*> selectedRows ;
    foreach(QTableWidgetItem* item, ui->dataTable->selectedItems())
        selectedRows += firstEntry(item) ;
    foreach(QTableWidgetItem* item, selectedRows)
        selectedPoints << pointInfo[item].value ;
    selectedCurve.setSamples(selectedPoints) ;

    // current point
    if (QTableWidgetItem* currentItem = ui->dataTable->currentItem())
        currentCurve.setSamples(QVector<QPointF>() << pointInfo[firstEntry(currentItem)].value);
    else
        currentCurve.setSamples(QVector<QPointF>());

    ui->itemPreview->replot();
    ui->metaPlot->replot();
}

QTableWidgetItem* metaItemProperties::firstEntry(QTableWidgetItem *item)
{
    return ui->dataTable->item(item->row(),0) ;
}

metaItemProperties::~metaItemProperties()
{
    clearItemInfo() ;
	delete ui;
}

void metaItemProperties::on_connectedItemsList_itemSelectionChanged()
{
    if (reselecting) return ;
    reselecting = true ;
//    ui->dataTable->clearSelection();
    QItemSelection selection ;
    foreach(QListWidgetItem* item, ui->connectedItemsList->selectedItems())
    {
        foreach(QTableWidgetItem* point, itemInfo[item].points)
        {
            QModelIndex index = ui->dataTable->model()->index(ui->dataTable->row(point),0) ;
            selection.append(QItemSelection(index, ui->dataTable->model()->index(index.row(),ui->dataTable->columnCount()-1)));
        }
    }
    ui->dataTable->selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
    refreshPlots();
    reselecting = false ;
}

void metaItemProperties::on_moveUpButton_clicked()
{
	moveSelection(false);
}

void metaItemProperties::on_moveDownButton_clicked()
{
	moveSelection(true) ;
}

void metaItemProperties::clearItemInfo()
{
    foreach(itemLink link, itemInfo)
        delete link.curve ;
    // clear all
    itemInfo.clear();
    modelItemInfo.clear();
    ui->connectedItemsList->clear();
}

void metaItemProperties::buildAssignments(const QList<specModelItem *>& items) // TODO change back to simple list w/o pair
{
    // save selection
    QVector<specModelItem*> selectedItems ;
    foreach(QListWidgetItem* listItem, ui->connectedItemsList->selectedItems())
        selectedItems << itemInfo[listItem].item ;

    clearItemInfo();

    // Construct item map and list widget
    for (QList<specModelItem*>::const_iterator i = items.begin() ; i != items.end() ; ++i)
    {
        // Add item to list widget
        QListWidgetItem* newItem = new QListWidgetItem((*i)->descriptor("")) ;
        newItem->setCheckState(Qt::Checked);
        ui->connectedItemsList->addItem(newItem) ;

        // prepare info struct
        itemLink info ;
        info.item = *i ;
        info.curve = new QwtPlotCurve ;
        QVector<QPointF> curveData ;
        for (size_t j = 0 ; j < (*i)->dataSize() ; ++j)
            curveData << (*i)->sample(j) ;
        info.curve->setSamples(curveData) ;

        // add entries to maps
        modelItemInfo[*i] = newItem ;
        itemInfo[newItem] = info ;
    }

    buildPoints();

    // restore selection
    QItemSelection selection ;
    foreach(specModelItem *modelItem, selectedItems)
    {
        QModelIndex itemIndex =
                ui->connectedItemsList->model()->index(
                    ui->connectedItemsList->row(modelItemInfo[modelItem]),0) ;
        selection.append(QItemSelectionRange());
    }
    ui->connectedItemsList->selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
}

void metaItemProperties::buildPoints()
{
    // clear all
    pointInfo.clear();
    ui->dataTable->clear();
    ui->dataTable->setRowCount(0);
    ui->dataTable->setColumnCount(2+originalItem->filter->evaluators.size()); // TODO fishy
    ui->dataTable->setHorizontalHeaderLabels(QStringList() << "x" << "y" << originalItem->filter->symbols);
    for (QMap<QListWidgetItem*,itemLink>::iterator i = itemInfo.begin() ; i != itemInfo.end() ; ++i)
        i->points.clear() ;

    // Construct point map and table widget
    QVector<specModelItem*> itemVector ;
    for (int i = 0 ; i < ui->connectedItemsList->count() ; ++i)
    {
        QListWidgetItem *listItem = ui->connectedItemsList->item(i) ;
        if (listItem->checkState() == Qt::Checked)
            itemVector << itemInfo[listItem].item ;
    }

    QVector<QVector<specModelItem*> > itemsPerPoint ;
    originalItem->filter->itemsToQuery(itemVector, itemsPerPoint) ;
    QVector<QPointF> evaluatedData ;
    for (int i = 0 ; i < itemsPerPoint.size() ; ++i)
    {
        // Do evaluation for one set of points
        QVector<QPointF> dataPoints ;
        QVector<QVector<double> > variableValues ;
        originalItem->filter->getVariableValues(itemsPerPoint[i], variableValues) ;
        originalItem->filter->getPoints(variableValues, dataPoints) ;

        // Prepare point info
        pointLink info ;
        foreach(specModelItem* modelItem, itemsPerPoint[i])
            info.items << modelItemInfo[modelItem] ;

        // add point to table and info vector
        for (int i = 0 ; i < dataPoints.size() ; ++i)
        {
            QTableWidgetItem *newTableEntry = new QTableWidgetItem(QString::number(dataPoints[i].x())) ;
            // add to list item info
            foreach(QListWidgetItem* listItem, info.items)
                itemInfo[listItem].points << newTableEntry ;

            // add point value to info
            info.value = dataPoints[i] ;

            // add variable values to table
            int row = ui->dataTable->rowCount() ;
            ui->dataTable->setRowCount(row+1) ;
            ui->dataTable->setItem(row,0, newTableEntry) ;
            ui->dataTable->setItem(row, 1, new QTableWidgetItem(QString::number(dataPoints[i].y()))) ;
            qDebug() << "variableValues.size:" << variableValues.size() ;
            for (int j = 0 ; j < variableValues[i].size() ; ++j)
                ui->dataTable->setItem(row, 2+j, new QTableWidgetItem(QString::number(variableValues[i][j])));

            pointInfo[newTableEntry] = info ;
        }
        // add to overall result
        evaluatedData << dataPoints ;
    }
    metaCurve.setSamples(evaluatedData);
}

void metaItemProperties::moveSelection(bool down)
{
    QList<QListWidgetItem*> selection = ui->connectedItemsList->selectedItems() ;
    if (selection.isEmpty()) return ;
    ui->connectedItemsList->selectionModel()->clearSelection();

    int begin = ui->connectedItemsList->row(selection.first()) ,
        end   = ui->connectedItemsList->row(selection.last())+1 ;
    if (down)
    {
        if(end == ui->connectedItemsList->count()) return ;
        for (int i = 0 ; i < end-begin ; ++i)
            ui->connectedItemsList->insertItem(i+1, ui->connectedItemsList->takeItem(begin+i));
    }
    else
    {
        if(0 == begin) return ;
        for (int i = 0 ; i < end-begin ; ++i)
            ui->connectedItemsList->insertItem(i-1, ui->connectedItemsList->takeItem(begin+i));
    }

    buildPoints();
    QItemSelection s ;
    foreach(QListWidgetItem* item, selection)
    {
        QModelIndex index = ui->connectedItemsList->model()->index(ui->connectedItemsList->row(item),0) ;
        s.append(QItemSelection(index,index)) ;
    }
    ui->connectedItemsList->selectionModel()->select(s, QItemSelectionModel::ClearAndSelect) ;
}

specUndoCommand* metaItemProperties::changedConnections(QObject *parent)
{
	specModel* model = qobject_cast<specModel*>(parent) ;
	if (!parent) return 0 ;
	QList<specModelItem*> newConnections ;
	for (int i = 0 ; i < ui->connectedItemsList->count() ; ++i)
    {
        QListWidgetItem* item = ui->connectedItemsList->item(i) ;
        if (item->checkState() == Qt::Checked)
            newConnections << itemInfo[item].item ;
    }

	specMultiCommand *parentCommand = new specMultiCommand ;
	parentCommand->setParentObject(parent) ;

	specDeleteConnectionsCommand *deleteCommand = new specDeleteConnectionsCommand(parentCommand) ;
	deleteCommand->setParentObject(parent) ;
    QModelIndexList indexList = generateConnectionList(originalItem->items) ;
    deleteCommand->setItems(model->index(originalItem), indexList) ;

	specAddConnectionsCommand *addCommand = new specAddConnectionsCommand(parentCommand) ;
	addCommand->setParentObject(parent) ;
	indexList = generateConnectionList(newConnections) ;
	addCommand->setItems(model->index(originalItem), indexList) ;
	parentCommand->setText(tr("Modify meta item propertis"));

	if (ui->styleFit->checkState() != originalItem->styleFitCurve)
	{
		specToggleFitStyleCommand* fitStyleCommand = new specToggleFitStyleCommand(parentCommand) ;
		fitStyleCommand->setParentObject(parent) ;
		fitStyleCommand->setup(model->index(originalItem));
	}

	if (!parentCommand->childCount())
	{
		delete parentCommand ;
		parentCommand = 0 ;
	}
	return parentCommand ;
}

QModelIndexList metaItemProperties::generateConnectionList(const QList<specModelItem*> items)
{
    QModelIndexList indexes ;
    foreach(specModelItem* item, items)
    {
        QModelIndex index = originalItem->metaModel->index(item) ;
        if (!index.isValid()) index = originalItem->dataModel->index(item) ;
        indexes << index ;
    }
    return indexes ;
}

void metaItemProperties::on_dataTable_itemSelectionChanged()
{
    if (reselecting) return ;
    reselecting = true ;
//    ui->connectedItemsList->clearSelection();
    QItemSelection selection ;
    foreach(QTableWidgetItem* point, ui->dataTable->selectedItems())
    {
        foreach(QListWidgetItem* item, pointInfo[firstEntry(point)].items)
        {
            QModelIndex index = ui->connectedItemsList->model()->index(ui->connectedItemsList->row(item),0) ;
            selection.append(QItemSelection(index, index)) ;
        }
    }
    ui->connectedItemsList->selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
    refreshPlots();
    reselecting = false ;
}

void metaItemProperties::on_connectedItemsList_itemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item)
    buildPoints();
    on_connectedItemsList_itemSelectionChanged();
}

void metaItemProperties::on_removeSelectedConnections_clicked()
{
    checkSelection(Qt::Unchecked) ;
}

void metaItemProperties::checkSelection(Qt::CheckState state)
{
    QList<QListWidgetItem*> selectedItems = ui->connectedItemsList->selectedItems() ;
    foreach(QListWidgetItem* item, selectedItems)
        item->setCheckState(state) ;
}

void metaItemProperties::on_addSelectedConnections_clicked()
{
    checkSelection(Qt::Checked);
}
