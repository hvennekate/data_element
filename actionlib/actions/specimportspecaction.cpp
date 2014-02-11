#include "specimportspecaction.h"
#include <QFileDialog>
#include "utility-functions.h"
#include "specaddfoldercommand.h"
#include "specview.h"
#include <QMessageBox>

specImportSpecAction::specImportSpecAction(QObject* parent) :
	specItemAction(parent)
{
	setIcon(QIcon::fromTheme("archive-insert"));
	setToolTip(tr("Import files")) ;
	setWhatsThis(tr("Import files.  Use this button to get started."));
	setText(tr("Import file...")) ;
	setShortcut(tr("Ctrl+F"));
}

specUndoCommand* specImportSpecAction::generateUndoCommand()
{
	int row = 0 ;
	if(!currentItem->isFolder())
	{
		row = currentIndex.row() + 1 ;
		currentIndex = currentIndex.parent() ;
	}

	QStringList fileNames = QFileDialog::getOpenFileNames(view, tr("Files to import")) ;   // TODO get proper file type from model
	if(fileNames.isEmpty()) return 0 ;
	QList<specModelItem*> importedItems ;
	for(int i = 0 ; i < fileNames.size() ; ++i)
	{
		QList<specModelItem*> (*importFunction)(QFile&) = fileFilter(fileNames[i]);
		if(!model->acceptableImportFunctions().contains(importFunction))
		{
			QMessageBox::critical(0, tr("Cannot import"), tr("Cannot import this type of data here:") + fileNames[i], QMessageBox::Ok) ;
			continue ;
		}
		QFile fileToImport(fileNames[i]) ;
		fileToImport.open(QFile::ReadOnly | QFile::Text) ;
		importedItems << importFunction(fileToImport) ;
	}
	if(importedItems.isEmpty()) return 0 ;

	if(! model->insertItems(importedItems, currentIndex, row))
	{
		foreach(specModelItem* item, importedItems)
			delete item ;
		return 0 ;
	}
	specAddFolderCommand* command = new specAddFolderCommand ;
	command->setParentObject(model) ;
	command->setItems(importedItems) ;
	command->setText((fileNames.size() > 1 ? tr("Import data (files:") : tr("Import data (file:"))
			 + fileNames.join(", ") + ")") ;
	return command ;
}

void specImportSpecAction::setFilters(const QStringList& f)
{
	filters = f ;
}
