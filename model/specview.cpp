#include "specview.h"
#include <specdelegate.h>
#include "specviewstate.h"
#include "specmodel.h"
#include "specactionlibrary.h"
#include <QContextMenuEvent>
#include <QHeaderView>

QModelIndexList specView::getSelection() const
{
	QModelIndexList l = selectedIndexes() ;
	QModelIndexList list ;
	foreach(QModelIndex index, l)
		if (!index.column())
			list << index ;
	qSort(list) ;
	return list ;
}

void specView::columnsInserted(const QModelIndex& parent, int start, int end)
{
	if(!model()) return ;
	if(!start) return ;
	if(end != model()->columnCount(parent) - 1) return ;
	int column = start - 1 ;
	resizeColumnToContents(column) ;
	if(!column) setColumnWidth(column, qMax(columnWidth(column), 50));
}

void specView::columnMoved(int i, int j, int k)
{
	QTreeView::columnMoved() ;
	//TODO
	if(k != i)
	{
		// TODO move to model
		header()->moveSection(k, i) ;  // undoing user move
		QList<QPair<QVariant, QVariant> > descriptors ;
		for(int a = qMin(j, k) ; a < qMax(j, k) + 1 ; a++)    // read out column heads of concern
			descriptors += qMakePair(model()->headerData(a, Qt::Horizontal),
						 model()->headerData(a, Qt::Horizontal, spec::MultiLineRole)) ;
		if(k < j) descriptors.prepend(descriptors.takeLast()) ;   // doing the move
		else descriptors.append(descriptors.takeFirst()) ;
		for(QList<QVariant>::size_type a = 0 ; a < descriptors.size() ; a++)  // reinserting the heads
		{
			model()->setHeaderData(qMin(j, k) + a, Qt::Horizontal, descriptors[a].first) ;
			model()->setHeaderData(qMin(j, k) + a, Qt::Horizontal, descriptors[a].second, spec::MultiLineRole) ;
		}
	}
}


void specView::triggerReselect()
{
	QItemSelection selected = selectionModel()->selection() ;
	selectionModel()->clearSelection() ;
	selectionModel()->select(selected, QItemSelectionModel::ClearAndSelect) ;  // reselect to emit selectionChanged-signal in order to invoke redrawing of plot
}

specModel* specView::model() const
{return (specModel*) QAbstractItemView::model() ;}

void specView::keyPressEvent(QKeyEvent* event)
{
	// TODO also enable mouse to take focus and eliminate selection
	if(event->key() == Qt::Key_Escape)
	{
		// TODO Extra function?
		clearSelection() ;
		setCurrentIndex(QModelIndex()) ;
	}

	else if(event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier)
	{
		QItemSelection newSelection ;
		foreach(QModelIndex index, selectionModel()->selectedRows())
		{
			if(!model()->isFolder(index))
				index = index.parent() ;
			else
				selectionModel()->select(QItemSelection(model()->index(index.row(), 0, index.parent()),
									model()->index(index.row(), model()->columnCount(index) - 1, index.parent())),
							 QItemSelectionModel::Deselect); // TODO deselect folder
			int columnCount = model()->columnCount(index) ;
			for(int i = 0 ; i < model()->rowCount(index) ; i++)
				newSelection.select(model()->index(i, 0, index), model()->index(i, columnCount - 1, index)) ;
		}
		selectionModel()->select(newSelection, QItemSelectionModel::Select) ;
	}

	else QTreeView::keyPressEvent(event) ;
}

specView::specView(QWidget* parent)
	: QTreeView(parent),
	  state(0),
	  actionLibrary(0)
{
	setAutoExpandDelay(500);
	setVerticalScrollMode(QAbstractItemView::ScrollPerItem);

	setAlternatingRowColors(true) ;
	setSelectionBehavior(QTreeView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection) ;
	setDragEnabled(true) ;
	setAcceptDrops(true) ;
	setDropIndicatorShown(true) ;
	QAbstractItemDelegate* olddel = itemDelegate() ;
	setItemDelegate(new specDelegate(this)) ;
	delete olddel ;
	setAllColumnsShowFocus(true) ;

	connect(header(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(columnMoved(int, int, int))) ;
}

void specView::contextMenuEvent(QContextMenuEvent* event)
{
	if(!indexAt(event->pos()).isValid())
	{
		clearSelection() ;
		setCurrentIndex(QModelIndex()) ;
	}
	if(!actionLibrary) return ;
	QMenu* cMenu = actionLibrary->contextMenu(this) ;
	cMenu->exec(event->globalPos()) ;
	delete cMenu ;
	event->accept() ;
}

void specView::setModel(specModel* model)
{
	QTreeView::setModel(model) ;
	// TODO change this to plain old functions.
	connect(model, SIGNAL(modelAboutToBeReset()), this, SLOT(prepareReset())) ;
	connect(model, SIGNAL(modelReset()), this, SLOT(resetDone())) ;
	connect(model, SIGNAL(columnsInserted(const QModelIndex&, int, int)), this, SLOT(columnsInserted(QModelIndex, int, int))) ;
}

specView::~specView()
{
	delete state ;
}

void specView::dropEvent(QDropEvent* event)
{
	bool internalMove = event->source() == this && event->proposedAction() == Qt::MoveAction ;
	model()->setInternalDrop(internalMove) ;
	if(internalMove)  // for some reason, we can't find out the topmost item in viewState if moving internally
		// moving externally and copying via drag'n'drop internally works fine...
		state = new specViewState(this) ;
	QTreeView::dropEvent(event) ;
}

void specView::writeToStream(QDataStream& out) const
{
	if(model())
		out << * ((specStreamable*) model()) ;  // TODO why is this cast necessary?
	specViewState state(this) ;
	out << state ;
}

void specView::readFromStream(QDataStream& in)
{
	if(model())
		in >> * ((specStreamable*) model()) ;  // TODO why is this cast necessary?
	specViewState state(this) ;
	in >> state ;
	state.restoreState();
}

void specView::prepareReset()
{
	if(!state)
		state = new specViewState(this) ;
}

void specView::resetDone()
{
	if(!state) return ;
	state->restoreState();
	delete state ;
	state = 0 ;
}

void specView::dragMoveEvent(QDragMoveEvent* event)
{
	QTreeView::dragMoveEvent(event) ;
	handleDropEventAction(event) ;
}

void specView::handleDropEventAction(QDropEvent* event)
{
	if(!acceptData(event->mimeData()))
	{
		event->ignore();
		return ;
	}
	if(qobject_cast<specView*> (event->source()) || event->proposedAction() == Qt::LinkAction)
	{
		event->acceptProposedAction();
		event->accept();
		return ;
	}
	if(event->possibleActions() & Qt::CopyAction)
	{
		event->setDropAction(Qt::CopyAction) ;
		event->accept();
		return ;
	}
	if(event->possibleActions() & Qt::LinkAction)
	{
		event->setDropAction(Qt::LinkAction) ;
		event->accept();
		return ;
	}
	event->ignore();
	return ;
}

void specView::dragEnterEvent(QDragEnterEvent* event)
{
	QTreeView::dragEnterEvent(event) ;
	handleDropEventAction(event) ;
}

bool specView::acceptData(const QMimeData* data)
{
	if(!model()) return false ;
	return model()->mimeAcceptable(data) ;
}

void specView::setActionLibrary(specActionLibrary* a)
{
	actionLibrary = a ;
}
