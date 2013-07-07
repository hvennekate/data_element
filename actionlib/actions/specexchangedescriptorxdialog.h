#ifndef SPECEXCHANGEDESCRIPTORXDIALOG_H
#define SPECEXCHANGEDESCRIPTORXDIALOG_H

#include <QDialog>

class stringListValidator ;

namespace Ui {
	class specExchangeDescriptorXDialog;
}

class specExchangeDescriptorXDialog : public QDialog
{
	Q_OBJECT

public:
	explicit specExchangeDescriptorXDialog(QWidget *parent = 0);
	QString descriptorToConvert() const ;
	QString newDescriptor() const ;
	void setDescriptors(const QStringList&) ;
	~specExchangeDescriptorXDialog();

private slots:
	void on_columnToConvert_currentIndexChanged(const QString &arg1) ;

private:
	Ui::specExchangeDescriptorXDialog *ui;
	QStringList entries ;
	stringListValidator *validator ;
};

#endif // SPECEXCHANGEDESCRIPTORXDIALOG_H
