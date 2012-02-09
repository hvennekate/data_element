#include "specimportspecaction.h"
#include <QFileDialog>
#include "utility-functions.h"
#include "specaddfoldercommand.h"
#include "specview.h"

specImportSpecAction::specImportSpecAction(QObject *parent) :
    specUndoAction(parent)
{
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
	qDebug("checking if item is folder") ;
	if (!item->isFolder())
	{
		qDebug("item is not a folder") ;
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
	command->setParentWidget(currentView) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
