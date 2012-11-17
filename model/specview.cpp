#include "specview.h"
#include <specdelegate.h>
#include "specviewstate.h"
#include "specmodel.h"
#include "specactionlibrary.h"
#include <QContextMenuEvent>
#include <QHeaderView>

QModelIndexList specView::getSelection()
{
	QModelIndexList list = selectionModel()->selectedRows() ;
	qSort(list) ;
	return list ;
}

void specView::columnsInserted(const QModelIndex &parent, int start, int end)
{
	if (!model()) return ;
	if (!start) return ;
	if (end != model()->columnCount(parent)-1) return ;
	int column = start -1 ;
	resizeColumnToContents(column) ;
	if (!column) setColumnWidth(column,qMax(columnWidth(column),50));
}

void specView::columnMoved(int i, int j, int k)
{
	QTreeView::columnMoved() ;
	//TODO
	if(k != i)
	{
		header()->moveSection(k,i) ; // undoing user move
		QList<QVariant> temp ;
		for (int a = qMin(j,k) ; a < qMax(j,k)+1 ; a++) // read out column heads of concern
			temp += model()->headerData(a,Qt::Horizontal) ;
		if(k < j) temp.prepend(temp.takeLast()) ; // doing the move
		else temp.append(temp.takeFirst()) ;
		for (QList<QVariant>::size_type a = 0 ; a < temp.size() ; a++) // reinserting the heads
			model()->setHeaderData(qMin(j,k)+a,Qt::Horizontal,temp[a]) ;
	}
}


void specView::triggerReselect()
{
	QItemSelection selected = selectionModel()->selection() ;
	selectionModel()->clearSelection() ;
	selectionModel()->select(selected,QItemSelectionModel::ClearAndSelect) ; // reselect to emit selectionChanged-signal in order to invoke redrawing of plot
}

specModel* specView::model() const
{return (specModel*) QAbstractItemView::model() ;}

void specView::keyPressEvent(QKeyEvent* event)
{  // TODO also enable mouse to take focus and eliminate selection
	if (event->key() == Qt::Key_Escape)
	{// TODO Extra function?
		clearSelection() ;
		setCurrentIndex(QModelIndex()) ;
	}
	
	else if (event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier)
	{
		QModelIndex index = currentIndex() ;
		if (!model()->isFolder(currentIndex()))
			index = index.parent() ;
		else
			selectionModel()->select(QItemSelection(model()->index(index.row(),0,index.parent()),
							       model()->index(index.row(),model()->columnCount(index)-1,index.parent())),
				QItemSelectionModel::Deselect); // TODO deselect folder
		QItemSelection newSelection ;
		int columnCount = model()->columnCount(index) ;
		for (int i = 0 ; i < model()->rowCount(index) ; i++)
			newSelection.select(model()->index(i,0,index),model()->index(i,columnCount-1,index)) ;
		selectionModel()->select(newSelection,QItemSelectionModel::Select) ;
	}

	else QTreeView::keyPressEvent(event) ;
}

specView::specView(QWidget* parent)
 : QTreeView(parent),
   actionLibrary(0),
   state(0)
{
	setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	
	setAlternatingRowColors(true) ;
    setSelectionBehavior(QTreeView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection) ;
	setDragEnabled(true) ;
	setAcceptDrops(true) ;
	setDropIndicatorShown(true) ;
	QAbstractItemDelegate *olddel = itemDelegate() ;
	setItemDelegate(new specDelegate) ;
	delete olddel ;
	setAllColumnsShowFocus(true) ;
	
	connect(header(),SIGNAL(sectionMoved(int,int,int)),this,SLOT(columnMoved(int,int,int))) ;
}

void specView::contextMenuEvent(QContextMenuEvent* event)
{
	if (!indexAt(event->pos()).isValid())
	{
		clearSelection() ;
		setCurrentIndex(QModelIndex()) ;
	}
	if (!actionLibrary) return ;
	QMenu *cMenu = actionLibrary->contextMenu(this) ;
	cMenu->exec(event->globalPos()) ;
	foreach (QAction* action, cMenu->actions())
		delete action ; // TODO check!
	delete cMenu ;
	event->accept() ;
}

void specView::setModel(specModel* model)
{
	QTreeView::setModel(model) ;
	// TODO change this to plain old functions.
	connect(model, SIGNAL(modelAboutToBeReset()), this, SLOT(prepareReset())) ;
	connect(model, SIGNAL(modelReset()), this, SLOT(resetDone())) ;
	connect(model,SIGNAL(columnsInserted(const QModelIndex&,int,int)), this, SLOT(columnsInserted(QModelIndex,int,int))) ;
}

specView::~specView()
{
	delete state ;
}

void specView::dropEvent(QDropEvent *event)
{
	model()->setInternalDrop((event->source() == this && event->proposedAction() == Qt::MoveAction)) ;
	model()->setDropSource(this) ;

	QTreeView::dropEvent(event) ;
}

void specView::writeToStream(QDataStream &out) const
{
	if (model())
		out << *((specStreamable*) model()) ; // TODO why is this cast necessary?
	specViewState state(this) ;
	out << state ;
}

void specView::readFromStream(QDataStream &in)
{
	if (model())
		in >> *((specStreamable*) model()) ; // TODO why is this cast necessary?
	specViewState state(this) ;
	in >> state ;
	state.restoreState();
}

void specView::prepareReset()
{
	state = new specViewState(this) ;
	state->getState();
}

void specView::resetDone()
{
	if (!state) return ;
	state->restoreState();
	delete state ;
	state = 0 ;
}

void specView::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeView::dragMoveEvent(event) ;
    if (!acceptData(event->mimeData()))
        event->ignore() ;
    else
        event->acceptProposedAction();
}

void specView::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeView::dragEnterEvent(event) ;
    if (!acceptData(event->mimeData()))
        event->ignore();
    else
        event->acceptProposedAction();
}

bool specView::acceptData(const QMimeData *data)
{
	if (!model()) return false ;
	return model()->mimeAcceptable(data) ;
}

void specView::setActionLibrary(specActionLibrary *a)
{
	actionLibrary = a ;
}
