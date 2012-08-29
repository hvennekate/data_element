#include "metaitemproperties.h"
#include "ui_metaitemproperties.h"
#include "actionlib/specaddconnectionscommand.h"
#include "actionlib/specdeleteconnectionscommand.h"
#include "actionlib/specmulticommand.h"

metaItemProperties::metaItemProperties(specMetaItem* i,QWidget *parent) :
	QDialog(parent),
	ui(new Ui::metaItemProperties),
	originalItem(i)
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

	if (newConnections == originalItem->items) return 0 ;

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
	qDebug() << "returning:" << parentCommand << "delete:" << deleteCommand << "add:" << addCommand ;
	qDebug() << "oldItems:" << originalItem->items << "new items:" << newConnections ;
	return parentCommand ;
}

