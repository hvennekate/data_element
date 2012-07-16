#include "specviewstate.h"

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
}

void specViewState::getState()
{
	if (!parent) return ;
	purgeLists();

	int columnCount = model()->columnCount(QModelIndex()) ;
	qDebug("++++ getting column widths %d",columnCount) ;
	for (int i = 0 ; i < columnCount ; ++i)
		widths << parent->columnWidth(i) ;

	qDebug("+++++ getting selection") ;
	QModelIndexList selectionList = parent->selectionModel()->selectedRows() ;
	for (int i = 0 ; i < selectionList.size() ; ++i)
		selectedItems << model()->itemPointer(selectionList[i]) ;

	qDebug("+++++ getting expanded") ;
	QModelIndex item = model()->index(0,0,QModelIndex()) ;
	while (item.isValid())
	{
		qDebug() << "+++++ Hierarchy: " << model()->hierarchy(item) << model()->rowCount(item.parent()) ;
		if (parent->isExpanded(item))
			openFolders << (specFolderItem*) model()->itemPointer(item) ;
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
	currentItem = model()->itemPointer(parent->currentIndex()) ;
	hierarchyOfCurrentItem = model()->hierarchy(currentItem) ;

	qDebug("+++++ getting topmost item") ;
	currentTopItem = model()->itemPointer(parent->indexAt(QPoint(0,0))) ;
	qDebug() << "++++++ item description:" << currentTopItem->descriptor("") ;
	hierarchyOfTopItem = model()->hierarchy(currentTopItem) ;


}

specModelItem* specViewState::hierarchyPointer(const QVector<int> &hierarchy)
{
	specModelItem* pointer = 0 ;
	int start = 0 ;
	while (start < hierarchy.size() && ! (pointer = model()->itemPointer(hierarchy.mid(start++)))) ;
	return pointer ;
}

void specViewState::restoreState()
{
	if (!parent) return ;


	currentTopItem = hierarchyPointer(hierarchyOfTopItem) ;
	currentItem = hierarchyPointer(hierarchyOfCurrentItem) ;

	qDebug("+++++ restoring view") ;

	// restore column widths
	qDebug("restoring column widths") ;
	for (int i = 0 ; i < widths.size() ; ++i)
		parent->setColumnWidth(i,widths[i]) ;

	// restore expanded folders
	qDebug("+++++ restoring expanded %d",openFolders.size()) ;
	for (int i = 0 ; i < openFolders.size() ; ++i)
		parent->expand(model()->index(openFolders[i])) ;

	// restore selection
	qDebug("+++++ restoring selection %d",selectedItems.size()) ;
	QItemSelection selectedIndexes ;
	for (int i = 0 ; i < selectedItems.size() ; ++i)
	{
		QModelIndex index = model()->index(selectedItems[i]) ;
		selectedIndexes.select(index,index) ;
	}
	qDebug("passing selection to model") ;
	parent->selectionModel()->select(selectedIndexes,QItemSelectionModel::Select) ;

	// restore current index
	qDebug() << "++++++ current index: " ;
	if (currentItem)
		parent->selectionModel()->setCurrentIndex(model()->index(currentItem),QItemSelectionModel::Current) ;
	qDebug() << "++++++ restored current item" ;

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
	quint32 openFoldersSize ;
	quint32 selectedItemsSize ;
	in >> openFoldersSize
	   >> selectedItemsSize
	   >> hierarchyOfTopItem
	   >> hierarchyOfCurrentItem
	   >> widths ;
	openFolders.resize(openFoldersSize) ;
	selectedItems.resize(selectedItemsSize);
	QVector<int> hierarchy ;
	if (!model())
	{
		purgeLists();
		return ; // TODO cause error here.
	}

	for (int i = 0 ; i < openFolders.size() ; ++i)
	{
		in >> hierarchy ;
		openFolders[i] = (specFolderItem*) model()->itemPointer(hierarchy) ;
	}
	for (int i = 0 ; i < selectedItems.size() ; ++i)
	{
		in >> hierarchy ;
		selectedItems[i] = model()->itemPointer(hierarchy) ;
	}

	// TODO maybe implicit restore
	return true ;
}
