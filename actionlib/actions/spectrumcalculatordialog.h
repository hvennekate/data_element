#ifndef SPECTRUMCALCULATORDIALOG_H
#define SPECTRUMCALCULATORDIALOG_H

#include <QDialog>

namespace Ui {
	class spectrumCalculatorDialog;
}

class spectrumCalculatorDialog : public QDialog
{
	Q_OBJECT

public:
	explicit spectrumCalculatorDialog(QWidget *parent = 0);
	~spectrumCalculatorDialog();

	QString xFormula() const ;
	QString yFormula() const ;
private slots:
	void on_toNm_clicked();

	void on_toAbsorption_clicked();

	void on_toTransmittance_clicked();

	void errorChanged(const QString&) ;

private:
	Ui::spectrumCalculatorDialog *ui;
};

#endif // SPECTRUMCALCULATORDIALOG_H
