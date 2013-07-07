#include "spectrumcalculatordialog.h"
#include "ui_spectrumcalculatordialog.h"
#include "specformulavalidator.h"
#include "QStringList"
#include <QCompleter>
#include <QStringListModel>

spectrumCalculatorDialog::spectrumCalculatorDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::spectrumCalculatorDialog),
	xFormulae(new QStringListModel(this)),
	yFormulae(new QStringListModel(this))
{
	ui->setupUi(this);
	QRegExp xy("(x|y|p\\d+)") ;
	specFormulaValidator *xValidator = new specFormulaValidator(xy, ui->formulaX) ;
	specFormulaValidator *yValidator = new specFormulaValidator(xy, ui->formulaY) ;
	ui->formulaX->setValidator(xValidator);
	ui->formulaY->setValidator(yValidator) ;
	connect(xValidator, SIGNAL(evaluationError(QString)), ui->errorsX, SLOT(setText(QString))) ;
	connect(yValidator, SIGNAL(evaluationError(QString)), ui->errorsY, SLOT(setText(QString))) ;
	connect(xValidator, SIGNAL(evaluationError(QString)), this, SLOT(errorChanged(QString))) ;
	connect(yValidator, SIGNAL(evaluationError(QString)), this, SLOT(errorChanged(QString))) ;

	connect(this, SIGNAL(accepted()), this, SLOT(syncFormulae())) ;

	ui->formulaX->setCompleter(new QCompleter(xFormulae, ui->formulaX)) ;
	ui->formulaY->setCompleter(new QCompleter(yFormulae, ui->formulaY));
	ui->formulaX->setText("x") ;
	ui->formulaY->setText("y");
}

void spectrumCalculatorDialog::syncFormulae() const
{
	syncFormula(ui->formulaX->text(), xFormulae) ;
	syncFormula(ui->formulaY->text(), yFormulae) ;
}

void spectrumCalculatorDialog::syncFormula(const QString &formula, QStringListModel *model) const
{
	QStringList formulae = model->stringList() ;
	if (formulae.contains(formula)) return ;
	formulae << formula ;
	model->setStringList(formulae) ;
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
