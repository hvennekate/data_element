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
    QList<QPair<specModelItem*, bool> > originalItems ;
    foreach(specModelItem* item, originalItem->serverList())
        originalItems << qMakePair(item, true) ;
    buildAssignments(originalItems);

	connect(ui->connectedItemsList, SIGNAL(itemSelectionChanged()), this, SLOT(refreshMetaPlot())) ;
	connect(ui->dataTable, SIGNAL(itemSelectionChanged()), this, SLOT(refreshMetaPlot())) ;
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
    currentCurve.setSamples(QVector<QPointF>() << pointInfo[ui->dataTable->currentItem()].value);

    ui->itemPreview->replot();
    ui->metaPlot->replot();
}

QTableWidgetItem* metaItemProperties::firstEntry(QTableWidgetItem *item)
{
    return ui->dataTable->item(item->row(),0) ;
}

metaItemProperties::~metaItemProperties()
{
	delete ui;
}

void metaItemProperties::on_connectedItemsList_itemSelectionChanged()
{
    if (reselecting) return ;
    reselecting = true ;
    ui->dataTable->clearSelection();
    foreach(QListWidgetItem* item, ui->connectedItemsList->selectedItems())
        foreach(QTableWidgetItem* point, itemInfo[item].points)
            ui->dataTable->selectRow(point->row()) ;
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

void metaItemProperties::buildAssignments(const QList<QPair<specModelItem *, bool> >&items) // TODO change back to simple list w/o pair
{
    // save selection
    QVector<specModelItem*> selectedItems ;
    foreach(QListWidgetItem* listItem, ui->connectedItemsList->selectedItems())
        selectedItems << itemInfo[ui->connectedItemsList->row(listItem)].item ;

    // clear all
    itemInfo.clear();
    ui->connectedItemsList->clear();

    // Construct item map and list widget
    QMap<specModelItem*, QListWidgetItem*> modelItemInfo ;
    for (QList<QPair<specModelItem*, bool> >::iterator i = items.begin() ; i != items.end() ; ++i)
    {
        // Add item to list widget
        QListWidgetItem* newItem = new QListWidgetItem(i->first->descriptor("")) ;
        newItem->setCheckState(i->second);
        ui->connectedItemsList->addItem(newItem) ;

        // prepare info struct
        itemLink info ;
        info.item = i->first ;
        info.curve = new QwtPlotCurve ;
        QVector<QPointF> curveData ;
        for (size_t i = 0 ; i < i->first->dataSize() ; ++i)
            curveData << i->first->sample(i) ;
        info.curve->setSamples(curveData) ;

        // add entries to maps
        modelItemInfo[i->first] = newItem ;
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
    for (QMap<QListWidgetItem*,itemLink>::iterator i = itemInfo.begin() ; i != itemInfo.end() ; ++i)
        i->value.points.clear() ;

    // Construct point map and table widget
    QVector<specModelItem*> itemVector ;
    for (QList<QPair<specModelItem*, bool> >::iterator i = items.begin() ; i != items.end() ; ++i)
        itemVector << i->first ;

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
            for (int j = 0 ; j < variableValues.size() ; ++j)
                ui->dataTable->setItem(row, 2+j, new QTableWidgetItem(QString::number(variableValues[j][i])));

            pointInfo << info ;
        }
        // add to overall result
        evaluatedData << dataPoints ;
    }
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
    ui->connectedItemsList->setSelection(s, QItemSelectionModel::ClearAndSelect) ;
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
    reselecting = false ;
}
