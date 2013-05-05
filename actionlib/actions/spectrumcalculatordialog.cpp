#include "spectrumcalculatordialog.h"
#include "ui_spectrumcalculatordialog.h"
#include "specformulavalidator.h"
#include "QStringList"

spectrumCalculatorDialog::spectrumCalculatorDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::spectrumCalculatorDialog)
{
	ui->setupUi(this);
	QStringList xy ;
	xy << "x" << "y" ;
	specFormulaValidator *xValidator = new specFormulaValidator(xy, ui->formulaX) ;
	specFormulaValidator *yValidator = new specFormulaValidator(xy, ui->formulaY) ;
	ui->formulaX->setValidator(xValidator);
	ui->formulaY->setValidator(yValidator) ;
	connect(xValidator, SIGNAL(evaluationError(QString)), ui->errorsX, SLOT(setText(QString))) ;
	connect(yValidator, SIGNAL(evaluationError(QString)), ui->errorsY, SLOT(setText(QString))) ;
	connect(xValidator, SIGNAL(evaluationError(QString)), this, SLOT(errorChanged(QString))) ;
	connect(yValidator, SIGNAL(evaluationError(QString)), this, SLOT(errorChanged(QString))) ;

	ui->formulaX->setText("x") ;
	ui->formulaY->setText("y");
}

spectrumCalculatorDialog::~spectrumCalculatorDialog()
{
	delete ui;
}

void spectrumCalculatorDialog::on_toNm_clicked()
{
	ui->formulaX->setText("1e7/x") ;
}

void spectrumCalculatorDialog::on_toAbsorption_clicked()
{
	ui->formulaY->setText("2-log(y)");
}

void spectrumCalculatorDialog::on_toTransmittance_clicked()
{
	ui->formulaY->setText("10^-y");
}

QString spectrumCalculatorDialog::xFormula() const
{
	return ui->formulaX->text() ;
}

QString spectrumCalculatorDialog::yFormula() const
{
	return ui->formulaY->text() ;
}

void spectrumCalculatorDialog::errorChanged(const QString &error)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(error.isEmpty());
}
