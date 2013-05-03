#include "specexchangedescriptorxdialog.h"
#include "ui_specexchangedescriptorxdialog.h"
#include "specdescriptoreditaction.h"

specExchangeDescriptorXDialog::specExchangeDescriptorXDialog(const QStringList &items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::specExchangeDescriptorXDialog),
    entries(items),
    validator(new stringListValidator(this))
{
    ui->setupUi(this);
    ui->xName->setValidator(validator);
    QString newLabel(tr("old x")) ;
    validator->fixup(newLabel) ;
    ui->xName->setText(newLabel);
    ui->columnToConvert->addItems(items) ;
}

specExchangeDescriptorXDialog::~specExchangeDescriptorXDialog()
{
    delete ui;
}

void specExchangeDescriptorXDialog::on_columnToConvert_currentIndexChanged(const QString &current)
{
    QStringList listContent(entries) ;
//    for (int i = 0 ; i < ui->columnToConvert->count() ; ++i)
//        listContent << ui->columnToConvert->itemText(i) ;
    listContent.removeOne(current) ;

    validator->setForbidden(listContent);
    ui->xName->setText(ui->xName->text());
}

QString specExchangeDescriptorXDialog::descriptorToConvert() const
{
    return ui->columnToConvert->currentText() ;
}

QString specExchangeDescriptorXDialog::newDescriptor() const
{
    return ui->xName->text() ;
}
