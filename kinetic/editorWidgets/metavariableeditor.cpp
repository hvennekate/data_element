#include "metavariableeditor.h"
#include "ui_metavariableeditor.h"
#include "specmetavariable.h"
#include <QDoubleValidator>

QStringList metaVariableEditor::plotDataTypes() const
{
	return QStringList() << tr("integral")
			     << tr("max y value")
			     << tr("min y value")
			     << tr("x values")
			     << tr("y values")
			     << tr("x of max y value")
			     << tr("x of min y value") ;
}

QStringList metaVariableEditor::typeSymbols() const
{
	return QStringList() << "i" << "u" << "l" << "x" << "y" << "p" << "P" ;
}

bool metaVariableEditor::isPlotVarType() const
{
	return plotDataTypes().contains(ui->valueSource->currentText()) ;
}

metaVariableEditor::metaVariableEditor(QWidget *parent) :
	abstractVariableEditor(parent),
	ui(new Ui::metaVariableEditor),
	minValidator(new QDoubleValidator),
	maxValidator(new QDoubleValidator)
{
	ui->setupUi(this);
	QDoubleValidator *validator = new QDoubleValidator(this) ;
	ui->xmin->setValidator(validator) ;
	ui->xmax->setValidator(validator) ;
	ui->useIndex->toggle() ;
}

metaVariableEditor::~metaVariableEditor()
{
	delete ui;
}

QString metaVariableEditor::variableDefinition() const
{
	QString result = ui->variableName->text() + " = " ;
	if (ui->useIndex->isChecked())
		result += "[" + ui->indexBegin->text()
				+ ":" + ui->indexEnd->text()
				+ ":" + ui->indexIncrement->text() + "]" ;
	if (isPlotVarType())
		result += typeSymbols().at(ui->valueSource->currentIndex())
				+ ui->xmin->text()
				+ ":" + ui->xmax->text() ;
	else
		result += ui->valueSource->currentText() ;
	return result ;
}

void metaVariableEditor::initialize(const QString &inString)
{
	QString string(inString) ;
	string.remove(QRegExp("\\s+")) ;
	ui->variableName->setText(string.section('=',0,0)) ;
	QString definition = string.section('=',1) ;
	QRegExp rangeExp("^\\[([^\\]:]*):?([^\\]:]*):?([^\\]:]*)\\]") ;
	ui->useIndex->setChecked(-1 != rangeExp.indexIn(definition));
	if (ui->useIndex->isChecked())
	{
		bool ok = false ;
		int v = rangeExp.cap(1).toInt(&ok) ;
		ui->indexBegin->setValue(ok ? v : 0);
		v = rangeExp.cap(2).toInt(&ok) ;
		ui->indexEnd->setValue(ok ? v : (int) INFINITY) ;
		v = rangeExp.cap(3).toInt(&ok) ;
		ui->indexIncrement->setValue(ok ? v : 1) ;
	}
	definition.remove(0,rangeExp.matchedLength()) ;
	if (!definition.isEmpty() && typeSymbols().contains(definition.left(1)))
	{
		ui->valueSource->setCurrentIndex(typeSymbols().indexOf(definition.left(1))) ;
		definition = definition.mid(1) ;
		ui->xmin->setText(definition.section(":",0,0)) ;
		ui->xmax->setText(definition.section(":",1,1)) ;
	}
	else
		ui->valueSource->setCurrentText(definition) ;
}

void metaVariableEditor::setData(const QVariant &data)
{
	QString oldValue = ui->valueSource->currentText() ;
	ui->valueSource->clear();
	ui->valueSource->addItems(plotDataTypes()) ;
	ui->valueSource->insertSeparator(ui->valueSource->count()) ;
	ui->valueSource->addItems(data.toStringList()
				  .replaceInStrings(QRegExp("^"), "\"")
				  .replaceInStrings(QRegExp("$"),"\"")) ;
}

void metaVariableEditor::on_valueSource_currentIndexChanged(const QString &typeName)
{
	Q_UNUSED(typeName)
	bool hasPlotVarType = isPlotVarType() ;
	QList<QWidget*> xminmaxWidgets ;
	xminmaxWidgets << ui->xminLabel
		       << ui->xmin
		       << ui->xmaxLabel
		       << ui->xmax ;
	foreach(QWidget* widget, xminmaxWidgets)
		widget->setVisible(hasPlotVarType);
}

void metaVariableEditor::sortMinMaxValues()
{
	double min = ui->xmin->text().toDouble(),
			max = ui->xmax->text().toDouble() ;
	if (max < min)
	{
		QString temp = ui->xmin->text() ;
		ui->xmin->setText(ui->xmax->text()) ;
		ui->xmax->setText(temp) ;
	}
}

