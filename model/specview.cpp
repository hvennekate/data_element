#include "specview.h"
#include <specmodelitem.h>
#include <QTextStream>
#include <QHeaderView>
#include <algorithm>
#include <QColorDialog>
#include <QPalette>
#include <specdelegate.h>
#include <QtAlgorithms>
#include "specfolderitem.h"
#include "cutbyintensitydialog.h"
#include <QSpinBox>
#include <QDialogButtonBox>
using std::min ;
using std::max ;


//TODO setSelectionBehavior to select only entire rows
//TODO: sort selection list before passing it on...

QModelIndexList specView::getSelection()
{
	QModelIndexList list = selectionModel()->selectedRows() ;
	qSort(list) ;
	return list ;
}

/*QList<specModelItem*> specView::currentlySelected()
{
	QList<specModelItem*> list ;
	QModelIndexList selection = getSelection() ;
	foreach(QModelIndex index, selection)
		list << (specModelItem*) index.internalPointer() ;
	return list ;
}*/

void specView::columnMoved(int i, int j, int k)
{
	QTreeView::columnMoved() ;
	//TODO
	if(k != i)
	{
		header()->moveSection(k,i) ; // undoing user move
		QList<QVariant> temp ;
		for (int a = min(j,k) ; a < max(j,k)+1 ; a++) // read out column heads of concern
			temp += model()->headerData(a,Qt::Horizontal) ;
		if(k < j) temp.prepend(temp.takeLast()) ; // doing the move
		else temp.append(temp.takeFirst()) ;
		for (QList<QVariant>::size_type a = 0 ; a < temp.size() ; a++) // reinserting the heads
			model()->setHeaderData(min(j,k)+a,Qt::Horizontal,temp[a]) ;
	}
}

void specView::buildTree()
{ model()->buildTree(currentIndex()) ;}

void specView::exportItems()
{
	QModelIndexList list = getSelection() ;
	model()->eliminateChildren(list) ;
	model()->exportData(list) ;
}

void specView::deleteItems()
{ 
	qDebug("triggered delete action") ;
	QModelIndexList list = getSelection() ;
	model()->eliminateChildren(list) ;
	selectionModel()->clearSelection() ;
	while (!list.empty())
	{
		QModelIndexList sameParent ;
		QModelIndex parent = model()->parent(list.first()) ;
		foreach(QModelIndex index, list)
			qDebug("orig line: %d",index.row()) ;
		sameParent << list.takeFirst() ;
		for (int i = 0 ; i < list.size() ; i++)
			if (model()->parent(list[i]) == parent)
				sameParent << list.takeAt(i--) ;
		qSort(sameParent) ;
		foreach(QModelIndex index, sameParent)
			qDebug("line: %d",index.row()) ;
		while(!sameParent.empty())
		{
			int last = sameParent.takeLast().row() ;
			int first = last ;
			while(sameParent.last().row() == first -1)
				first = sameParent.takeLast().row() ;
			model()->removeRows(first,last-first+1,parent) ;
		}
	}
}

void specView::triggerReselect()
{
	QItemSelection selected = selectionModel()->selection() ;
	selectionModel()->clearSelection() ;
	selectionModel()->select(selected,QItemSelectionModel::ClearAndSelect) ; // reselect to emit selectionChanged-signal in order to invoke redrawing of plot
}

bool specView::setAllSelected(const QVariant & value, int role)
{
	bool retVal = true ;
	QModelIndexList list = getSelection() ;
	foreach(QModelIndex index, list)
		retVal = retVal && model()->setData(index,value,role) ;
	triggerReselect() ;
	return retVal ;
}

specModel* specView::model() const
{return (specModel*) QAbstractItemView::model() ;}

void specView::mergeFolder()
{ setAllSelected(true,33) ; }

void specView::mergeItems() // TODO maybe change currentIndex
{
	QModelIndexList list = getSelection() ;
	selectionModel()->select(QModelIndex(),QItemSelectionModel::Clear) ;
	list = model()->mergeItems(list) ;
	// TODO reselect merged items
// 	qDebug("selecting merged indexes") ;
// 	QItemSelection newSelection ;
// 	qDebug("preparing selection") ;
// 	foreach(QModelIndex index, list)
// 	{
// 		qDebug("selecting index") ;
// 		newSelection.select(index,index.model()->index(index.row(),index.model()->columnCount(index.parent())-1,index.parent())) ;
// 	}
// 	qDebug("applying selection") ;
// 	selectionModel()->select(newSelection,QItemSelectionModel::Select) ;
// 	
// // 	foreach(QModelIndex index, list)
// // 		selectionModel()->select(
// // 			QItemSelection(index, model()->index(index.row(),model()->columnCount(index)-1,index.parent())),
// // 			QItemSelectionModel::Select) ;
// 	qDebug("Done selecting") ;
}

void specView::changePen()
{
	if (getSelection().isEmpty()) return ; // TODO connect to selectionChanged signal
	QPen pen(QColorDialog::getColor()) ;
	setAllSelected(pen,Qt::ForegroundRole) ;
}

void specView::newFolder()
{ model()->insertItems(QList<specModelItem*>() << new specFolderItem(),currentIndex(),0) ; }

void specView::keyPressEvent(QKeyEvent* event)
{  // TODO also enable mouse to take focus and eliminate selection
	if (event->key() == Qt::Key_Escape)
	{// TODO Extra function?
		clearSelection() ;
		setCurrentIndex(QModelIndex()) ;
	}
	
	if (event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier)
	{
		qDebug("Checking if folder") ;
		QModelIndex index = currentIndex() ;
		if (!model()->isFolder(currentIndex()))
			index = index.parent() ;
		else
			selectionModel()->select(QItemSelection(model()->index(index.row(),0,index.parent()),
							       model()->index(index.row(),model()->columnCount(index)-1,index.parent())),
				QItemSelectionModel::Deselect); // TODO deselect folder
		QItemSelection newSelection ;
		int columnCount = model()->columnCount(index) ;
		qDebug("preparing new selection") ;
		for (int i = 0 ; i < model()->rowCount(index) ; i++)
			newSelection.select(model()->index(i,0,index),model()->index(i,columnCount-1,index)) ;
		qDebug("selecting") ;
		selectionModel()->select(newSelection,QItemSelectionModel::Select) ;
		qDebug("done selecting") ;
	}
	
	if (event->key() == Qt::Key_Delete)
		deleteAction->trigger() ;
}

specView::specView(QWidget* parent)
 : QTreeView(parent),
   dropBuddy(0)
{
	createActions() ;
	createContextMenus() ;
	
	setAlternatingRowColors(true) ;
	setSelectionMode(QAbstractItemView::ExtendedSelection) ;
	setDragEnabled(true) ;
	setAcceptDrops(true) ;
	setDropIndicatorShown(true) ;
	setItemDelegate(new specDelegate) ;
	setAllColumnsShowFocus(true) ;
	
	connect(header(),SIGNAL(sectionMoved(int,int,int)),this,SLOT(columnMoved(int,int,int))) ;
}

void specView::createContextMenus()
{
	itemContextMenu = new QMenu(this) ;
	itemContextMenu->addAction(deleteAction) ;
	itemContextMenu->addAction(changePenAction) ;

	folderContextMenu = new QMenu(this) ;
	folderContextMenu->addAction(deleteAction) ;
	folderContextMenu->addAction(treeAction) ;
	folderContextMenu->addAction(changePenAction) ;
	folderContextMenu->addAction(mergeFolderAction) ;
}

QList<QAction*> specView::actions()
{
	return QList<QAction*>() << newItemAction << deleteAction << treeAction << changePenAction << mergeFolderAction << mergeAction << exportAction << cutByIntensityAction << averageAction << movingAverageAction << getSubtractionDataAction << applySubtractionAction;
}

void specView::contextMenuEvent(QContextMenuEvent* event)
{
	if (!indexAt(event->pos()).isValid())
	{
		clearSelection() ;
		setCurrentIndex(QModelIndex()) ;
	}
	bool containsOnlyFolders = true ;
	QModelIndexList list = getSelection() ;
	foreach(QModelIndex index, list)
		containsOnlyFolders = containsOnlyFolders && model()->isFolder(index) ;
	
	(containsOnlyFolders ? folderContextMenu : itemContextMenu )->exec(event->globalPos()) ;
	event->accept() ;
}

void specView::setModel(specModel* model)
{
	QTreeView::setModel(model) ;
}

void specView::createActions()
{// TODO connect actions to modified slot of parent
	deleteAction = new QAction(QIcon(":/item_delete.png"), tr("&Löschen"), this) ;
	deleteAction->setShortcut(tr("Entf"));
	deleteAction->setStatusTip(tr("Markierte Einträge löschen"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItems()));
	
	newItemAction = new QAction(QIcon(":/folder_new.png"), tr("&Neuer Eintrag"), this);
	newItemAction->setShortcut(tr("Ctrl+E"));
	newItemAction->setStatusTip(tr("Neuen Eintrag anlegen"));
	connect(newItemAction, SIGNAL(triggered()), this, SLOT(newFolder()));
	
	treeAction = new QAction(QIcon(":/tree.png"), tr("Baum aufbauen"), this) ;
	treeAction->setShortcut(tr("Ctrl+T"));
	treeAction->setStatusTip(tr("Elemente kategorisieren"));
	connect(treeAction, SIGNAL(triggered()), this, SLOT(buildTree())) ;
	
	changePenAction = new QAction(QIcon(":/color.png"), tr("Plot-Stil ändern"), this) ;
	changePenAction->setShortcut(tr("Ctrl+P"));
	changePenAction->setStatusTip(tr("Linienfarbe einstellen"));
	connect(changePenAction, SIGNAL(triggered()), this, SLOT(changePen())) ;
	
	mergeFolderAction = new QAction(QIcon(":/merge_folder.png"), tr("Plots zusammenfassen"), this) ;
	mergeFolderAction->setShortcut(tr("Ctrl+M"));
	mergeFolderAction->setStatusTip(tr("Plots der Einträge zusammenfassen"));
	connect(mergeFolderAction, SIGNAL(triggered()), this, SLOT(mergeFolder())) ;
	
	mergeAction = new QAction(QIcon(":/merge.png"), tr("Einträge vereinen"), this) ;
	mergeAction->setShortcut(tr("Ctrl+M")) ;
	mergeAction->setStatusTip(tr("Einträge (ggf. in Verzeichnis) zusammenführen")) ;
	connect(mergeAction, SIGNAL(triggered()), this, SLOT(mergeItems())) ;
	
	exportAction = new QAction(QIcon(":/export.png"), tr("Daten exportieren"), this) ;
	exportAction->setShortcut(tr("Ctrl+E")) ;
	exportAction->setStatusTip(tr("Daten exportieren")) ;
	connect(exportAction,SIGNAL(triggered()),this,SLOT(exportItems())) ;

	cutByIntensityAction = new QAction(QIcon(":/cbi.png"), tr("Nach Intensität beschneiden"), this) ;
	connect(cutByIntensityAction,SIGNAL(triggered()),this,SLOT(cutByIntensity())) ;

	averageAction = new QAction(QIcon(":/ave.png"), tr("Mitteln"), this) ;
	connect(averageAction,SIGNAL(triggered()),this,SLOT(averageItems())) ;

	movingAverageAction = new QAction(QIcon(":/mave.png"), tr("Laufend mitteln"), this) ;
	connect(movingAverageAction,SIGNAL(triggered()),this,SLOT(averageItems())) ;

	getSubtractionDataAction = new QAction(QIcon(":/newMinus.png"), tr("Subtraktionsdaten setzen"), this) ;
	connect(getSubtractionDataAction, SIGNAL(triggered()), this, SLOT(currentlySelectedToSubMap())) ;

	applySubtractionAction = new QAction(QIcon(":/multiminus.png"), tr("Subtraktion anwenden"), this) ;
	connect(applySubtractionAction,SIGNAL(triggered()), this, SLOT(applySubMapToSelection())) ;
}

specView::~specView()
{
}

void specView::cutByIntensity()
{
	QModelIndexList list = getSelection() ;
	if (list.isEmpty())
		return ;
	QList<specModelItem*> items ;
	foreach(QModelIndex index, list)
		items << (specModelItem*) (index.internalPointer()) ;
	cutByIntensityDialog *dialog = new cutByIntensityDialog(this) ;
	dialog->assignSpectra(items) ;
	dialog->setModal(true) ;
	dialog->exec() ;
	if(dialog->result() == QDialog::Accepted)
		dialog->performDeletion();
	delete dialog ;
}

void specView::averageItems()
{
	QDialog *numberDialog = new QDialog(this) ;
	QVBoxLayout *dialogLayout = new QVBoxLayout(numberDialog) ;
	QSpinBox *spinBox = new QSpinBox(numberDialog) ;
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,numberDialog) ;

	dialogLayout->addWidget(spinBox) ;
	dialogLayout->addWidget(buttons) ;

	spinBox->setMinimum(1) ;
	numberDialog->setWindowTitle("Wieviele mitteln?") ;
	numberDialog->setLayout(dialogLayout) ;

	connect(buttons,SIGNAL(accepted()),numberDialog,SLOT(accept())) ;
	connect(buttons,SIGNAL(rejected()),numberDialog,SLOT(reject())) ;

	qDebug("executing dialog") ;
	numberDialog->exec() ;
	qDebug("dialog done") ;

	if (numberDialog->result() == QDialog::Accepted)
	{
		QModelIndexList list = getSelection() ;
		if (sender() == averageAction)
			foreach(QModelIndex index, list)
				((specModelItem*) index.internalPointer())->average(spinBox->value()) ;
		else if (sender() == movingAverageAction)
			foreach(QModelIndex index, list)
				((specModelItem*) index.internalPointer())->movingAverage(spinBox->value()) ;
	}

	delete numberDialog ;
	qDebug("done averaging") ;
}

void specView::currentlySelectedToSubMap()
{
	model()->fillSubMap(getSelection());
}

void specView::applySubMapToSelection()
{
	model()->applySubMap(getSelection()) ;
}

void specView::setDropBuddy(specActionLibrary *lib)
{
	dropBuddy = lib ;
}

void specView::dropEvent(QDropEvent *event)
{
	if (!dropBuddy)
		QTreeView::dropEvent(event) ;
	else
		dropBuddy->dragDrop(event,this) ;
}
