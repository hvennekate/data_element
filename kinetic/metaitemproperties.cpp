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
	refreshing(false)
{
	ui->setupUi(this);
	ui->moveUpButton->setIcon(QIcon::fromTheme("go-up"));
	ui->moveDownButton->setIcon(QIcon::fromTheme("go-down")) ;

	ui->itemPreview->setAutoDelete(false) ;

	int num = 0 ;
	foreach(specModelItem* item, i->items)
	{
		QListWidgetItem *newItem = new QListWidgetItem(item->descriptor("")) ;
		newItem->setCheckState(Qt::Checked) ;
		ui->connectedItemsList->addItem(newItem) ;
		connectedData << qMakePair(num++, new QwtPlotCurve) ;
		QVector<QPointF> samples ; // TODO ueberarbeiten
		for (size_t i = 0 ; i < item->dataSize() ; ++i)
			samples << item->sample(i) ;
		connectedData.last().second->setSamples(samples);
	}

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

	int count = 0 ;
	foreach(specModelItem* item, i->items)
	{
		QwtSeriesData<QPointF> *itemData = i->filter->evaluate(QVector<specModelItem*>()<< item);
		int total = itemData->size() ;
		delete itemData ;
		for (int j = 0 ; j < total ; ++j)
			pointToItem << count ;
		count++ ;
	}

	ui->dataTable->setRowCount(metaCurve.dataSize()) ;
	for (size_t i = 0 ; i < metaCurve.dataSize() ; ++i)
	{
		QPointF sample = metaCurve.sample(i) ;
		ui->dataTable->setItem(i,0,new QTableWidgetItem(QString::number(sample.x()))) ;
		ui->dataTable->setItem(i,1,new QTableWidgetItem(QString::number(sample.y()))) ;
	}

	connect(ui->connectedItemsList, SIGNAL(itemSelectionChanged()), this, SLOT(refreshMetaPlot())) ;
	connect(ui->dataTable, SIGNAL(itemSelectionChanged()), this, SLOT(refreshMetaPlot())) ;
}

void metaItemProperties::refreshMetaPlot()
{
	if (!sender()) return ;
	if (refreshing) return ;
	refreshing = true ;
	QVector<QPointF> selectedSamples ;
	if (sender() == ui->connectedItemsList)
	{
		QItemSelection pointSelection ;
		foreach(QListWidgetItem* item, ui->connectedItemsList->selectedItems())
		{
			int index = ui->connectedItemsList->row(item) ;
			for (QVector<int>::iterator i = pointToItem.begin() ; i != pointToItem.end() ; i)
			{
				i = qFind(i, pointToItem.end(), index) ;
				if (i != pointToItem.end())
				{
					QModelIndex pointIndex = ui->dataTable->model()->index(i-pointToItem.begin(),0) ;
					pointSelection.append(QItemSelectionRange(pointIndex, pointIndex)) ;
					++i ;
				}
			}
		}
		ui->dataTable->selectionModel()->select(pointSelection, QItemSelectionModel::ClearAndSelect) ;
	}
	else
	{
		QItemSelection itemSelection ;
		foreach(QTableWidgetItem* item, ui->dataTable->selectedItems()) // TODO might double select
		{
			QModelIndex itemIndex = ui->connectedItemsList->model()->index(pointToItem[ui->dataTable->row(item)], 0) ; // TOD dangerous...
			itemSelection.append(QItemSelectionRange(itemIndex, itemIndex)) ;
		}
		ui->connectedItemsList->selectionModel()->select(itemSelection,QItemSelectionModel::ClearAndSelect);
	}

	// highlight points
	foreach(QTableWidgetItem* item, ui->dataTable->selectedItems()) // TODO might double select
		selectedSamples << metaCurve.sample(ui->dataTable->row(item)) ;
	selectedCurve.setSamples(selectedSamples) ;
	if (ui->dataTable->currentRow() >= 0)
		currentCurve.setSamples(QVector<QPointF>() << metaCurve.sample(ui->dataTable->currentRow()));
	ui->metaPlot->replot() ;
	refreshing = false ;
}

// TODO include variable view

metaItemProperties::~metaItemProperties()
{
	delete ui;
}

void metaItemProperties::on_connectedItemsList_itemSelectionChanged()
{
	ui->itemPreview->detachItems(QwtPlotItem::Rtti_PlotItem, false) ;

	foreach(QListWidgetItem* item, ui->connectedItemsList->selectedItems())
		connectedData[ui->connectedItemsList->row(item)].second->attach(ui->itemPreview) ;
	ui->itemPreview->replot();
}

void metaItemProperties::on_moveUpButton_clicked()
{
	moveSelection(false);
}

void metaItemProperties::on_moveDownButton_clicked()
{
	moveSelection(true) ;
}

void metaItemProperties::moveSelection(bool down)
{
	if (ui->connectedItemsList->selectedItems().isEmpty()) return ;

	int begin = ui->connectedItemsList->row(ui->connectedItemsList->selectedItems().first()),
	    end   = ui->connectedItemsList->row(ui->connectedItemsList->selectedItems().last())+1 ;

	if ((begin == 0 && !down) || (end == connectedData.size() && down)) return ;

	if (down)
	{
		connectedData = connectedData.mid(0,begin) +
				connectedData.mid(end,1) +
				connectedData.mid(begin,end-begin) +
				connectedData.mid(end+1) ;
		ui->connectedItemsList->insertItem(begin,ui->connectedItemsList->takeItem(end)) ;

	}
	else
	{
		connectedData = connectedData.mid(0,begin-1) +
				connectedData.mid(begin,end-begin) +
				connectedData.mid(begin-1,1) +
				connectedData.mid(end) ;
		ui->connectedItemsList->insertItem(end-1,ui->connectedItemsList->takeItem(begin-1)) ;
	}
}

QModelIndexList metaItemProperties::generateConnectionList(const QList<specModelItem*>& items)
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

specUndoCommand* metaItemProperties::changedConnections(QObject *parent)
{
	specModel* model = qobject_cast<specModel*>(parent) ;
	if (!parent) return 0 ;
	QList<specModelItem*> newConnections ;
	for (int i = 0 ; i < ui->connectedItemsList->count() ; ++i)
		if (ui->connectedItemsList->item(i)->checkState() == Qt::Checked)
			newConnections << originalItem->items[connectedData[i].first] ;

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

