#include "dataitemproperties.h"
#include "ui_dataitemproperties.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "specexchangefiltercommand.h"

dataItemProperties::dataItemProperties(specDataItem* i, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::dataItemProperties),
	item(*i),
	originalItem(i),
	setupComplete(false)
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

	setupComplete = true ;
	refreshPlot();
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
	bool dataUnchanged = true, correctionUnchanged = true ;
	if(item.data.size() == originalItem->data.size())
		for(size_t i = 0 ; i < item.dataSize() ; ++i)
			dataUnchanged = dataUnchanged &&
					item.data[i].exactlyEqual(originalItem->data[i]) ;
	else dataUnchanged = false ;

	correctionUnchanged =
	    (item.dataFilter() == originalItem->dataFilter()) ;

	if(correctionUnchanged && dataUnchanged) return 0 ;

	specUndoCommand* parentMulti = 0 ;
	if(!dataUnchanged && !correctionUnchanged)
	{
		parentMulti = new specMultiCommand ;
		parentMulti->setParentObject(parent) ;
	}

	if(!dataUnchanged)
	{
		specExchangeDataCommand* dataCommand = new specExchangeDataCommand(parentMulti) ;
		dataCommand->setParentObject(parent);
		dataCommand->setItem(originalItem, item.data) ;
		if(correctionUnchanged)
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
		if(dataUnchanged)
			return correctionCommand ;
	}
	parentMulti->setText(tr("Modify item data and correction")) ;
	return parentMulti ;
}
