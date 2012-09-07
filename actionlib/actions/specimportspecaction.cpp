#include "specimportspecaction.h"
#include <QFileDialog>
#include "utility-functions.h"
#include "specaddfoldercommand.h"
#include "specview.h"

specImportSpecAction::specImportSpecAction(QObject *parent) :
    specUndoAction(parent)
{
	setIcon(QIcon(":/fileimport.png"));
	setToolTip(tr("Import files")) ;
	setWhatsThis(tr("Import data files.  Use this button to get started by directly importing data."));
}

const std::type_info &specImportSpecAction::possibleParent()
{
	return typeid(specView) ;
}

void specImportSpecAction::execute()
{
	specView *currentView = (specView*) parent() ; // TODO: outsource this code (shared with insert folder action)
	specModel *model = currentView->model() ;
	QModelIndex index = currentView->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	if (!item->isFolder())
	{
		row = index.row()+1 ;
		index = index.parent() ;

	}

	QStringList fileNames = QFileDialog::getOpenFileNames(currentView,tr("Files to import")) ; // TODO get proper file type from model
	QList<specModelItem*> importedItems ;
	for(int i = 0 ; i < fileNames.size() ; ++i)
	{
		QList<specModelItem*> (*importFunction)(QFile&) = fileFilter(fileNames[i]);
		if (!importFunction) continue ;
		QFile fileToImport(fileNames[i]) ;
		fileToImport.open(QFile::ReadOnly | QFile::Text) ;
		importedItems << importFunction(fileToImport) ;
	}

	if (! model->insertItems(importedItems, index, row)) return ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	QModelIndexList newIndexes ;
	for (int i = 0 ; i < importedItems.size() ; ++i)
		newIndexes << model->index(row+i,0,index) ;
	command->setItems(newIndexes) ;
	command->setParentObject(currentView->model()) ;
	command->setText("Import data") ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
