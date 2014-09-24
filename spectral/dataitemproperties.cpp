#include "dataitemproperties.h"
#include "ui_dataitemproperties.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "specexchangefiltercommand.h"
#include "utility-functions.h"
#include "specdelegate.h"
#include "speceditdescriptorcommand.h"
#include "specmultilinecommand.h"

dataItemProperties::dataItemProperties(specDataItem* i, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::dataItemProperties),
	item(*i),
	originalItem(i),
	setupComplete(false),
	buildingDescriptorTable(false),
	descriptorDelegate(new specDelegate(this))
{
	if(!i) return ;
	item.sortPlotData = false ;
	item.mergePlotData = false ;
	ui->setupUi(this);
	ui->slopeValue->setRange(-INFINITY, INFINITY);
	ui->xShiftValue->setRange(-INFINITY, INFINITY);
	ui->offsetValue->setRange(-INFINITY, INFINITY);
	ui->scalingValue->setRange(-INFINITY, INFINITY);
	item.setPen(QPen()) ;
	item.setSymbol(0) ;

	highlightSelected.setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
				    QBrush(Qt::blue),
				    QPen(Qt::NoPen),
				    QSize(5, 5)));
	highlightSelected.setStyle(QwtPlotCurve::NoCurve);
	highlightCurrent.setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
				   QBrush(Qt::red),
				   QPen(Qt::NoPen),
				   QSize(5, 5)));
	highlightCurrent.setStyle(QwtPlotCurve::NoCurve);
	highlightCurrent.setZ(INFINITY);

	ui->dataPreview->setAutoDelete(false) ;
	item.attach(ui->dataPreview);

	specDataPointFilter filter = item.filter ;
	ui->slopeValue->setValue(filter.getSlope()) ;
	ui->offsetValue->setValue(filter.getOffset()) ;
	ui->xShiftValue->setValue(filter.getXShift()) ;
	ui->scalingValue->setValue(filter.getFactor()) ;


	ui->dataWidget->setRowCount(item.data.size()) ;
	ui->dataWidget->setColumnCount(3) ;
	for(int i = 0 ; i < item.data.size() ; ++i)
	{
		ui->dataWidget->
		setItem(i, 0, new QTableWidgetItem(QString::number(item.data[i].nu)));
		ui->dataWidget->
		setItem(i, 1, new QTableWidgetItem(QString::number(item.data[i].sig)));
		ui->dataWidget->
		setItem(i, 2, new QTableWidgetItem(QString::number(item.data[i].mint)));
	}

	getDescriptors();
	ui->descriptorTable->setItemDelegateForColumn(1, descriptorDelegate) ;

	setupComplete = true ;
	refreshPlot();
}

void dataItemProperties::getDescriptors()
{
	buildingDescriptorTable = true ;
	ui->descriptorTable->clearContents();
	ui->descriptorTable->setSortingEnabled(false) ;
	QStringList descriptors = item.descriptorKeys() ;
	foreach(const QString& descriptor, descriptors)
		addDescriptor(descriptor,
			      item.descriptor(descriptor, true),
			      item.isMultiline(descriptor)) ;
	ui->descriptorTable->sortByColumn(0, Qt::AscendingOrder);
	ui->descriptorTable->setSortingEnabled(true) ;
	buildingDescriptorTable = false ;
}

QIcon dataItemProperties::numericIcon(const specDescriptor &d)
{
	return d.isNumeric() ? QIcon::fromTheme("dialog-ok-apply") : QIcon::fromTheme("dialog-cancel") ;
}

dataItemProperties::~dataItemProperties()
{
	delete ui;
}

void dataItemProperties::refreshPlot()
{
	if(!setupComplete) return ;
	// get correction from ui
	item.setDataFilter(specDataPointFilter(
			       ui->offsetValue->value(),
			       ui->slopeValue->value(),
			       ui->scalingValue->value(),
			       ui->xShiftValue->value()));

	// get data from table

	for(int i = 0 ; i < item.data.size() ; ++i)
	{
		item.data[i].nu   = ui->dataWidget->item(i, 0)->text().toDouble() ;
		item.data[i].sig  = ui->dataWidget->item(i, 1)->text().toDouble() ;
		item.data[i].mint = ui->dataWidget->item(i, 2)->text().toDouble() ;
	}

	item.revalidate();

	QVector<QPointF> selectedData ;
	foreach(QTableWidgetItem * selectedItem, ui->dataWidget->selectedItems())
	selectedData << item.sample(selectedItem->row()) ;
	highlightSelected.setSamples(selectedData) ;
	highlightCurrent.setSamples(
	    ui->dataWidget->currentItem() ?
	    QVector<QPointF>() << item.sample(ui->dataWidget->currentRow()) :
	    QVector<QPointF>()) ;
	connectCurve(highlightSelected) ;
	connectCurve(highlightCurrent) ;
	ui->dataPreview->replot();
}

void dataItemProperties::connectCurve(QwtPlotCurve& c)
{
	if(c.dataSize()) c.attach(ui->dataPreview) ;
	else c.detach();
}

specUndoCommand* dataItemProperties::changeCommands(QObject* parent)
{
	bool dataUnchanged = true,
			correctionUnchanged = true,
			descriptorsUnchanged = true ;

	// Checking data
	if(item.data.size() == originalItem->data.size())
		for(size_t i = 0 ; i < item.dataSize() ; ++i)
			dataUnchanged = dataUnchanged &&
					item.data[i].exactlyEqual(originalItem->data[i]) ;
	else dataUnchanged = false ;

	// Checking filter
	correctionUnchanged =
	    (item.dataFilter() == originalItem->dataFilter()) ;

	// Checking descriptors
	QStringList oldDescriptors = originalItem->descriptorKeys() ;
	oldDescriptors.sort(Qt::CaseInsensitive);

#define GETDESCRIPTORFROMTABLELOOP(INNERCODE) \
	for (int i = 0 ; i < ui->descriptorTable->rowCount() ; ++i) \
	{ \
		QString descriptor = ui->descriptorTable->item(i,0)->text() ; \
		QString content = ui->descriptorTable->item(i,1)->text() ; \
		bool multiline = (ui->descriptorTable->item(i,2)->checkState() == Qt::Checked) ; \
		INNERCODE \
	}

	if (oldDescriptors == tableDescriptors())
	{
		GETDESCRIPTORFROMTABLELOOP(if (originalItem->descriptor(descriptor, true) != content || originalItem->isMultiline(descriptor) != multiline) { descriptorsUnchanged = false ; break ; })
	}
	else
		descriptorsUnchanged = false ;

	if(correctionUnchanged
			&& dataUnchanged
			&& descriptorsUnchanged) return 0 ;

	specUndoCommand* parentMulti = 0 ;
	if((int) dataUnchanged + (int) correctionUnchanged + (int) descriptorsUnchanged < 2)
	{
		parentMulti = new specMultiCommand ;
		parentMulti->setParentObject(parent) ;
	}



	if (!descriptorsUnchanged)
	{
		specMultiCommand *descriptorCommand = new specMultiCommand(parentMulti) ;
		descriptorCommand->setParentObject(parent);
		QSet<QString> newDescriptors ;
		GETDESCRIPTORFROMTABLELOOP(
			newDescriptors << descriptor ;

			if (!item.descriptorKeys().contains(descriptor) || content != item.descriptor(descriptor, true))
			{
				specEditDescriptorCommand *edit = new specEditDescriptorCommand(descriptorCommand) ;
				edit->setItem(originalItem, descriptor, content, originalItem->activeLine(descriptor)) ;
				if (!item.descriptorKeys().contains(descriptor)) // new descriptor
					edit->setDeleteOnEmpty();
			}
			if (originalItem->isMultiline(descriptor) != multiline)
			{
				specMultiLineCommand *mlCommand = new specMultiLineCommand(descriptor, descriptorCommand) ;
				mlCommand->setItems(QList<specModelItem*>() << originalItem) ;
			}
		)

		foreach(const QString& descriptor, item.descriptorKeys().toSet() - newDescriptors)
		{
			specEditDescriptorCommand *deleteDescriptorCommand = new specEditDescriptorCommand(descriptorCommand) ;
			deleteDescriptorCommand->setDeleteOnEmpty() ;
			deleteDescriptorCommand->setItem(originalItem, descriptor, "");
		}

		if (!parentMulti)
		{
			descriptorCommand->setText(tr("Modify descriptors."));
			return descriptorCommand ;
		}
	}

	if(!dataUnchanged)
	{
		specExchangeDataCommand* dataCommand = new specExchangeDataCommand(parentMulti) ;
		dataCommand->setParentObject(parent);
		dataCommand->setItem(originalItem, item.data) ;
		if(!parentMulti)
		{
			dataCommand->setText(tr("Modify item data"));
			return dataCommand ;
		}
	}

	if(!correctionUnchanged)
	{
		specExchangeFilterCommand* correctionCommand = new specExchangeFilterCommand(parentMulti) ;
		correctionCommand->setParentObject(parent) ;
		correctionCommand->setItem(originalItem) ;
		correctionCommand->setAbsoluteFilter(item.filter) ;
		if(!parentMulti)
			return correctionCommand ;
	}
	parentMulti->setText(tr("Modify item data and correction")) ;
	return parentMulti ;
}

void dataItemProperties::on_descriptorTable_cellChanged(int row, int column)
{
	if (buildingDescriptorTable) return ;
	buildingDescriptorTable = true ;
	if (0 == column)
	{
		QStringList descriptors = tableDescriptors() ;
		QString newDescriptor = descriptors.takeAt(row) ;
		ui->descriptorTable->item(row,0)->setText(uniqueString(newDescriptor, descriptors)) ;
	}
	if (1 == column)
	{
		specDescriptor d ;
		d.setAppropriateContent(ui->descriptorTable->item(row, column)->text()) ;
		ui->descriptorTable->item(row, 3)->setIcon(numericIcon(d));
	}
	buildingDescriptorTable = false ;
	if (!tableDescriptors().contains(""))
		addDescriptor("", "", false) ;
}

void dataItemProperties::on_removeDescriptorButton_clicked()
{
	QList<int> rows ;
	foreach (QTableWidgetItem* item, ui->descriptorTable->selectedItems())
		rows << ui->descriptorTable->row(item) ;
	rows = rows.toSet().toList() ;
	qSort(rows) ;
	while (!rows.isEmpty())
		ui->descriptorTable->removeRow(rows.takeLast());
	if (!tableDescriptors().contains(""))
		addDescriptor("", "", false) ;
}

QStringList dataItemProperties::tableDescriptors() const
{
	QStringList descriptors ;
	for (int i = 0 ; i < ui->descriptorTable->rowCount() ; ++i)
	{
		QTableWidgetItem *item = ui->descriptorTable->item(i,0) ;
		if (item) descriptors << item->text() ;
	}
	return descriptors ;
}

int dataItemProperties::addDescriptor(const QString &name, const QString &content, bool multiline)
{
	bool prevBuild = buildingDescriptorTable ;
	buildingDescriptorTable = true ;
	bool sorting = ui->descriptorTable->isSortingEnabled() ;
	ui->descriptorTable->setSortingEnabled(false) ;
	ui->descriptorTable->insertRow(0);
	ui->descriptorTable->setItem(0,0,new QTableWidgetItem(name)) ;
	ui->descriptorTable->setItem(0,1,new QTableWidgetItem(content)) ;

	QTableWidgetItem *multilineItem = new QTableWidgetItem ;
	multilineItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled) ;
	multilineItem->setCheckState(multiline ? Qt::Checked : Qt::Unchecked) ;
	ui->descriptorTable->setItem(0,2,multilineItem) ;

	QTableWidgetItem *numericIndicator = new QTableWidgetItem(numericIcon(specDescriptor(content)),"") ;
	numericIndicator->setFlags(Qt::NoItemFlags);
	ui->descriptorTable->setItem(0,3,numericIndicator) ;

	ui->descriptorTable->setSortingEnabled(sorting) ;
	buildingDescriptorTable = prevBuild ;
	return ui->descriptorTable->row(numericIndicator) ;
}

void dataItemProperties::on_addDescriptorButton_clicked()
{
	int row = addDescriptor(uniqueString(tr("New tag"), tableDescriptors()), "", false) ;
	QTableWidgetItem* item = ui->descriptorTable->item(row,1) ;
	if (!item) return ;
	ui->descriptorTable->scrollToItem(item);
	ui->descriptorTable->editItem(item);
}
