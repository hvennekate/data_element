#include "dataitemproperties.h"
#include "ui_dataitemproperties.h"
#include <qwt_series_data.h>
#include "actionlib/specmulticommand.h"
#include "actionlib/specexchangedatacommand.h"
#include "actionlib/specplotmovecommand.h"

dataItemProperties::dataItemProperties(specDataItem *i, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::dataItemProperties),
	item(*i),
	originalItem(i),
	setupComplete(false)
{
	ui->setupUi(this);
	ui->slopeValue->setRange(-INFINITY,INFINITY);
	ui->xShiftValue->setRange(-INFINITY,INFINITY);
	ui->offsetValue->setRange(-INFINITY,INFINITY);
	ui->scalingValue->setRange(-INFINITY,INFINITY);
	item.setPen(QPen()) ;
	item.setSymbol(0) ;

	highlightSelected.setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
						  QBrush(Qt::blue),
						  QPen(Qt::NoPen),
						  QSize(5,5)));
	highlightSelected.setStyle(QwtPlotCurve::NoCurve);
	highlightCurrent.setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
						 QBrush(Qt::red),
						 QPen(Qt::NoPen),
						 QSize(5,5)));
	highlightCurrent.setStyle(QwtPlotCurve::NoCurve);

	ui->dataPreview->setAutoDelete(false) ;
	item.attach(ui->dataPreview);
	highlightSelected.attach(ui->dataPreview) ;
	highlightCurrent.attach(ui->dataPreview) ;

	ui->slopeValue->setValue(item.slope) ;
	ui->offsetValue->setValue(item.offset) ;
	ui->xShiftValue->setValue(item.xshift) ;
	ui->scalingValue->setValue(item.factor) ;


	ui->dataWidget->setRowCount(item.data.size()) ;
	ui->dataWidget->setColumnCount(3) ;
	for (int i = 0 ; i < item.data.size() ; ++i)
	{
		ui->dataWidget->
		      setItem(i,0, new QTableWidgetItem(QString::number(item.data[i].nu)));
		ui->dataWidget->
		      setItem(i,1, new QTableWidgetItem(QString::number(item.data[i].sig)));
		ui->dataWidget->
		      setItem(i,2, new QTableWidgetItem(QString::number(item.data[i].mint)));
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
	if (!setupComplete) return ;
	// get correction from ui
	item.invalidate();
	item.slope = ui->slopeValue->value() ;
	item.xshift = ui->xShiftValue->value() ;
	item.offset = ui->offsetValue->value() ;
	item.factor = ui->scalingValue->value() ;

	// get data from table
	qDebug() << ui->dataWidget->item(0,1) ;

	for (int i = 0 ; i < item.data.size() ; ++i)
	{
		item.data[i].nu   = ui->dataWidget->item(i,0)->text().toDouble() ;
		item.data[i].sig  = ui->dataWidget->item(i,1)->text().toDouble() ;
		item.data[i].mint = ui->dataWidget->item(i,2)->text().toDouble() ;
	}

	item.revalidate();

	QVector<QPointF> selectedData ;
	foreach(QTableWidgetItem* selectedItem, ui->dataWidget->selectedItems())
		selectedData << item.sample(selectedItem->row()) ;
	highlightSelected.setSamples(selectedData) ;
	highlightCurrent.setSamples(QVector<QPointF>() <<
				    item.sample(ui->dataWidget->currentRow())) ;

	ui->dataPreview->replot();
}

specUndoCommand* dataItemProperties::changeCommands(QObject* parent)
{
	bool dataUnchanged = true, correctionUnchanged = true ;
	if (item.data.size() == originalItem->data.size())
		for (size_t i = 0 ; i < item.dataSize() ; ++i)
			dataUnchanged = dataUnchanged &&
				    item.data[i].exactlyEqual(originalItem->data[i]) ;
	else dataUnchanged = false ;

	correctionUnchanged = (item.slope == originalItem->slope &&
			       item.offset == originalItem->offset &&
			       item.xshift == originalItem->xshift &&
			       item.factor == originalItem->factor) ;
	specUndoCommand *parentMulti = 0 ;
	if (!dataUnchanged && !correctionUnchanged)
	{
		parentMulti = new specMultiCommand ;
		parentMulti->setParentObject(parent) ;
	}

	QModelIndex itemIndex = ((specModel*) parent)->index(originalItem) ;

	if (!dataUnchanged)
	{
		specExchangeDataCommand* dataCommand = new specExchangeDataCommand(parentMulti) ;
		dataCommand->setParentObject(parent);
		dataCommand->setItem(itemIndex,item.data) ;
		if (correctionUnchanged) return dataCommand ;
	}

	if (!correctionUnchanged)
	{
		specPlotMoveCommand *correctionCommand = new specPlotMoveCommand(parentMulti) ;
		correctionCommand->setParentObject(parent) ;
		correctionCommand->setItem(itemIndex);

		// TODO this gets ugly.  Make interface more direct!
		double newFactor = item.factor/originalItem->factor ;
		correctionCommand->setCorrections(item.xshift - originalItem->xshift,
						  item.offset - originalItem->offset * newFactor,
						  item.slope  - originalItem->slope  * newFactor,
						  newFactor);
		if (dataUnchanged) return correctionCommand ;
	}
	return parentMulti ;
}
