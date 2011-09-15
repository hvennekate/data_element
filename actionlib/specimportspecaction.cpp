#include "specimportspecaction.h"

specImportSpecAction::specImportSpecAction(QObject *parent) :
    specUndoAction(parent)
{
}

const std::type_info specImportSpecAction::possibleParent()
{
	return typeId(specView) ;
}

void specImportSpecAction::execute()
{
	specDataView *currentView = (specDataView*) parent() ; // TODO: outsource this code (shared with insert folder action)
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
	if (! model->insertItems(QList<specModelItem*>() << new specFolderItem(), index, row)) return ;

}
