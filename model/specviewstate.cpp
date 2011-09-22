#include "specview.h"

specViewState::specViewState(specView *Parent)
{
	setParent(Parent) ;
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
	geometry.clear();
}

void specViewState::getState()
{
	if (!parent) return ;
	purgeLists();

	geometry = parent->saveGeometry() ;

	int columnCount = model()->columnCount(QModelIndex()) ;
	qDebug("++++ getting column widths %d",columnCount) ;
	for (int i = 0 ; i < columnCount ; ++i)
		widths << columnWidth(i) ;

	qDebug("+++++ getting selection") ;
	QModelIndexList selectionList = selectionModel()->selectedRows() ;
	for (int i = 0 ; i < selectionList.size() ; ++i)
		selectedItems << model()->itemPointer(selectionList[i]) ;

	qDebug("+++++ getting expanded") ;
	QModelIndex item = model()->index(0,0,QModelIndex()) ;
	while (item.isValid())
	{
		qDebug() << "+++++ Hierarchy: " << model()->hierarchy(item) << model()->rowCount(item.parent()) ;
		if (isExpanded(item))
			openFolders << model()->itemPointer(item) ;
		// go into if directory
		if (model()->rowCount(item))
			item = model()->index(0,0,item) ;
		// go up if reached last item
		else //if (model()->rowCount(item.parent()) == item.row()+1)
		{
			// if last item go up.
			while (model()->rowCount(item.parent()) == item.row()+1 && item.isValid())
				item = item.parent() ;
			// go to next
			item = item.sibling(item.row()+1,0);
		}
	}

	qDebug("+++++ getting current") ;
	// get current index
	currentItem = model()->hierarchy(currentIndex()) ;
	hierarchyOfCurrentItem = model()->hierarchy(currentItem) ;

	qDebug("+++++ getting topmost item") ;
	currentTopItem = model()->itemPointer(parent->indexAt(QPoint(0,0))) ;
	qDebug() << "++++++ item description:" << currentTopItem->descriptor("") ;
	hierarchyOfTopItem = model()->hierarchy(currentTopItem) ;


}

void specViewState::restoreState()
{
	if (!parent) return ;

	qDebug("+++++ restoring view") ;

	parent->restoreGeometry(geometry) ;

	// restore column widths
	qDebug("restoring column widths") ;
	for (int i = 0 ; i < widths.size() ; ++i)
		setColumnWidth(i,widths[i]) ;

	// restore expanded folders
	qDebug("+++++ restoring expanded %d",openFolders.size()) ;
	for (int i = 0 ; i < openFolders.size() ; ++i)
		parent->expand(model()->index(openFolders[i])) ;

	// restore selection
	qDebug("+++++ restoring selection") ;
	QItemSelection selectedIndexes ;
	for (int i = 0 ; i < selectedItems.size() ; ++i)
	{
		QModelIndex index = model()->index(selectedItems[i]) ;
		selectedIndexes.select(index,index) ;
	}
	parent->selectionModel()->select(selectedIndexes,QItemSelectionModel::Select) ;

	// restore current index
	parent->selectionModel()->setCurrentIndex(model()->index(current),QItemSelectionModel::Current) ;

	//scroll to topmost item
	parent->scrollTo(model()->index(currentTopItem),QAbstractItemView::PositionAtTop) ;
}

QDataStream& specViewState::write(QDataStream &out)
{
	return out ;
}

QDataStream& specViewState::read(QDataStream &in)
{
	return in ;
}
