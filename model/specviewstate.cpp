#include "specviewstate.h"
#include <QHeaderView>

specViewState::specViewState(specView *Parent)
{
	setParent(Parent) ;
}

specViewState::specViewState(const specView *temp)
	: parent(0)
{
	getState(temp) ;
}

void specViewState::setParent(specView *Parent)
{
	parent = Parent ;
	getState() ;
}

void specViewState::purgeLists()
{
	openFolders.clear();
	selectedItems.clear();
	currentTopItem = 0 ;
	currentItem = 0 ;
	hierarchyOfTopItem.clear();
	widths.clear();
}

void specViewState::getState()
{
	getState(parent) ;
}

void specViewState::getState(const specView* view)
{
	if (!view) return ;
	purgeLists();
	specModel* model = view->model() ;

	int columnCount = view->header()->count() ;  //model->columnCount(QModelIndex()) ;
	for (int i = 0 ; i < columnCount ; ++i)
		widths << view->columnWidth(i) ;

	QModelIndexList selectionList = view->selectionModel()->selectedRows() ; // TODO: Performance
	for (int i = 0 ; i < selectionList.size() ; ++i)
		selectedItems << model->itemPointer(selectionList[i]) ;

	QModelIndex item = model->index(0,0,QModelIndex()) ;
	while (item.isValid())
	{
		if (view->isExpanded(item))
			openFolders << (specFolderItem*) model->itemPointer(item) ;
		// go into if directory
		if (model->rowCount(item))
			item = model->index(0,0,item) ;
		// go up if reached last item
		else //if (model->rowCount(item.parent()) == item.row()+1)
		{
			// if last item go up.
			while (model->rowCount(item.parent()) == item.row()+1 && item.isValid())
				item = item.parent() ;
			// go to next
			item = item.sibling(item.row()+1,0);
		}
	}

	// get current index
	currentItem = model->itemPointer(view->currentIndex()) ;
	hierarchyOfCurrentItem = model->hierarchy(currentItem) ;

	currentTopItem = model->itemPointer(view->indexAt(QPoint(0,0))) ;
	hierarchyOfTopItem = model->hierarchy(currentTopItem) ;
}

specModelItem* specViewState::hierarchyPointer(const QVector<int> &hierarchy)
{
	specModelItem* pointer = 0 ;
	int start = 0 ;
	while (start < hierarchy.size() && ! (pointer = model()->itemPointer(hierarchy.mid(start++)))) ;
	return pointer ;
}

QItemSelection specViewState::pointersToSelection(const QVector<specModelItem*>& selectedItems, const specModel* model)
{
	QModelIndexList indexes ;
	foreach(specModelItem* item, selectedItems)
		indexes << model->index(item) ;
	return indexesToSelection(indexes, model) ;
}

QItemSelection specViewState::indexesToSelection(const QModelIndexList& selectedItems, const specModel* model)
{
	QItemSelection selectedIndexes ;
	foreach(const QModelIndex index, selectedItems)
		selectedIndexes << QItemSelectionRange(index,
						       model->index(index.row(),
								    model->columnCount(index)-1,
								    index.parent()))  ;
	return selectedIndexes ;
}

void specViewState::restoreState()
{
	if (!parent) return ;


	currentTopItem = hierarchyPointer(hierarchyOfTopItem) ;
	currentItem = hierarchyPointer(hierarchyOfCurrentItem) ;


	// restore column widths
	for (int i = 0 ; i < widths.size() ; ++i)
		parent->setColumnWidth(i,widths[i]) ;

	// restore current index
	if (currentItem)
		parent->selectionModel()->setCurrentIndex(model()->index(currentItem),QItemSelectionModel::Current) ;

	// restore selection
	parent->selectionModel()->select(pointersToSelection(selectedItems,model()),QItemSelectionModel::Select) ;

	// restore expanded folders
	parent->collapseAll();
	for (int i = 0 ; i < openFolders.size() ; ++i)
		parent->expand(model()->index(openFolders[i])) ;

	//scroll to topmost item
	if (currentTopItem)
		parent->scrollTo(model()->index(currentTopItem),QAbstractItemView::PositionAtTop) ;
}

void specViewState::writeToStream(QDataStream &out) const
{
	out << quint32(openFolders.size())
	    << quint32(selectedItems.size())
	    << hierarchyOfTopItem
	    << hierarchyOfCurrentItem
	    << widths ;
	for (int i = 0 ; i < openFolders.size() ; ++i)
		out << model()->hierarchy(openFolders[i]) ;
	for (int i = 0 ; i < selectedItems.size() ; ++i)
		out << model()->hierarchy(selectedItems[i]) ;
}

void specViewState::readFromStream(QDataStream &in)
{
	openFolders.clear();
	selectedItems.clear();
	quint32 openFoldersSize ;
	quint32 selectedItemsSize ;
	in >> openFoldersSize
	   >> selectedItemsSize
	   >> hierarchyOfTopItem
	   >> hierarchyOfCurrentItem
	   >> widths ;
	QVector<int> hierarchy ;
	if (!model())
	{
		purgeLists();
		return ; // TODO cause error here.
	}

	for (quint32 i = 0 ; i < openFoldersSize ; ++i)
	{
		in >> hierarchy ;
		specFolderItem* p = dynamic_cast<specFolderItem*>(model()->itemPointer(hierarchy)) ;
		if (p) openFolders << p ;
	}
	for (quint32 i = 0 ; i < selectedItemsSize ; ++i)
	{
		in >> hierarchy ;
		specModelItem *p = model()->itemPointer(hierarchy) ;
		if (p) selectedItems << p ;
	}
	// TODO maybe implicit restore
}
