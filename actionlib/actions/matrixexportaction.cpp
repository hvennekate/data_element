#include "matrixexportaction.h"
#include "ui_matrixexportdialog.h"
#include "specmatrixmimeexporter.h"
#include <QClipboard>

matrixExportAction::matrixExportAction(QObject *parent) :
	specRequiresItemAction(parent),
	dialog(new Ui::matrixExportDialog),
	d(new QDialog)
{
	setIcon(QIcon(":/matrix-export.png")) ;
	setToolTip(tr("Copy matrix...")) ;
	setWhatsThis(tr("Create a matrix of x values and copy to clipboard"));
	setText(tr("Copy matrix...")) ;

	dialog->setupUi(d);
}

matrixExportAction::~matrixExportAction()
{
	delete dialog ;
	delete d ;
}

specUndoCommand* matrixExportAction::generateUndoCommand()
{
	QString selected = dialog->descriptor->currentText() ;
	dialog->descriptor->clear() ;
	dialog->descriptor->addItems(model->descriptors());
	dialog->descriptor->setCurrentText(selected);

	if (QDialog::Accepted != d->exec()) return 0 ;

	specMatrixMimeExporter exporter ;
	exporter.setDescriptor(dialog->descriptor->currentText());
	exporter.setXAcross(dialog->xAcross->isChecked()) ;
	exporter.setSortY(dialog->sortY->isChecked());
	QStringList separators ;
	separators << " " << "\t" << "," << ";" ;
	exporter.setSeparator(separators[dialog->separator->currentIndex()]);

	QApplication::clipboard()->clear();
	QApplication::clipboard()->setText(exporter.matrix(pointers));

	return 0 ;
}
