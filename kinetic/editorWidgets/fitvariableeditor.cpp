#include "fitvariableeditor.h"
#include "ui_fitvariableeditor.h"

fitVariableEditor::fitVariableEditor(QWidget *parent) :
	abstractVariableEditor(parent),
	ui(new Ui::fitVariableEditor)
{
	ui->setupUi(this);
	ui->variableValue->setValidator(new QDoubleValidator(this)) ;
}

fitVariableEditor::~fitVariableEditor()
{
	delete ui;
}

QString fitVariableEditor::variableDefinition() const
{
	QString result = ui->variableName->text() ;
	if (ui->tFactor->isChecked())
		result += "/" + QString::number(ui->confidenceInterval->value()) ;
	result += "=" + ui->variableValue->text() ;
	return result ;
}

void fitVariableEditor::initialize(const QString &def)
{
	QRegExp variableRegExp("^\\s*(\\w+)\\s*(/\\s*(0?\\.\\d+))?\\s*=?\\s*([+-]?\\d*\\.?\\d*([eEdD][+-]?\\d*)?)") ; // TODO combine with specFitCurve
	if (variableRegExp.indexIn(def)) return ;

	ui->variableName->setText(variableRegExp.cap(1)) ;
	ui->variableValue->setText(variableRegExp.cap(4)) ;
	ui->tFactor->setChecked(!variableRegExp.cap(2).isEmpty());
	if (ui->tFactor->isChecked())
		ui->confidenceInterval->setValue(variableRegExp.cap(3).toDouble());
}
