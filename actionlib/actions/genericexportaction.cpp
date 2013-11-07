#include "genericexportaction.h"
#include "exportdialog.h"
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

genericExportAction::genericExportAction(QObject* parent) :
	specRequiresItemAction(parent),
	exportFormat(new exportDialog(0))
{
	setIcon(QIcon(":/export.png")) ;
	setToolTip(tr("Export to ASCII")) ;
	setWhatsThis(tr("Export selected items to ASCII file.")) ;
	setText(tr("Export ASCII..."));
	setShortcut(tr("e"));
}

genericExportAction::~genericExportAction()
{
	delete exportFormat;
}

specUndoCommand* genericExportAction::generateUndoCommand()
{
	QFile exportFile(QFileDialog::getSaveFileName(0, "File name", "", "ASCII files (*.asc)")) ;
	if(exportFile.fileName() == "") return 0 ;

	exportFormat->setDataTypes(model->dataTypes()) ;
	exportFormat->setDescriptors(model->descriptors()) ;
	if(exportFormat->exec() != QDialog::Accepted) return 0 ;

	if(!exportFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(0, tr("Cannot open file"), tr("Cannot open file\n") + exportFile.fileName(), QMessageBox::Ok) ;
		return 0 ;
	}
	QTextStream out(&exportFile) ;
	QList<QPair<bool, QString> > headerFormat = exportFormat->headerFormat() ;
	QList<QPair<spec::value, QString> > dataFormat = exportFormat->dataFormat() ;
	if(selection.isEmpty())
		selection << QModelIndex() ;
	QList<specModelItem*> pointers = model->pointerList(selection) ;
	foreach(specModelItem * item, pointers)
	item->exportData(headerFormat, dataFormat, out) ;
	exportFile.close() ;
	return 0 ;
}
