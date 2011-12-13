#include "specplotwidget.h"
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <QLabel>
#include <QFont>
#include <QFontDialog>
#include <QFileDialog>
#include <utility-functions.h>
#include <QTextStream>
#include <specdelegate.h>
#include <QContextMenuEvent>
#include <QFileInfo>
#include <QDataStream>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>
#include <names.h>
#include <QSettings>

void specPlotWidget::changeFileName(const QString& name)
{
	onDisk->setFileName(name) ;
	setWindowTitle(QFileInfo(name).fileName()) ;
	kineticWidget->setWindowTitle(QString("Kinetics of ").append(QFileInfo(name).fileName())) ;
}

void specPlotWidget::contextMenuEvent(QContextMenuEvent* event)
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
	int a,b,c,d ;
	splitter->geometry().getCoords(&a,&b,&c,&d) ;
// 	cout << "Splitter:  x: " << a << " " << c << "  y: " << b << " " << d << endl ;
	if(splitter->geometry().contains(event->x(),event->y()))
	{
		if (splitter->orientation() == Qt::Horizontal)
			splitter->setOrientation(Qt::Vertical) ;
		else
			splitter->setOrientation(Qt::Horizontal) ;
	}
}

specPlotWidget::specPlotWidget(const QString& fileName, QWidget *parent)
: QDockWidget(QFileInfo(fileName).fileName(), parent)
{
	kineticWidget = new specKineticWidget(QString(),this) ;

	onDisk  = new QFile() ;
	changeFileName(fileName) ;
	QDataStream in ;
	if(onDisk->exists())
	{
		onDisk->open(QFile::ReadOnly) ;
		in.setDevice(onDisk) ;
	}
	content = new QWidget ;
	layout  = new QVBoxLayout ;
	plot    = new specSpectrumPlot ;
	plot->setMinimumHeight(100) ;
	
	items   = new specDataView(this) ;
	
	qDebug("starting to read file") ;
	if (onDisk->exists())
	{
		qDebug() << "filepos:" << onDisk->pos() ;
		in >> *kineticWidget ;
		qDebug() << "filepos:" << onDisk->pos() ;
	}
	
	qDebug("read kinetic model") ;
	
	items->setModel(onDisk->exists() ? new specModel(in,items) : new specModel(items) ) ; // TODO employ >> operator
	plot->setView(items) ;
	qDebug("model: %d",items->model()) ;
	qDebug() << "filepos:" << onDisk->pos() ;
	
	qDebug("read both data and kinetics models") ;
	
	qDebug() << "connecting to model" << kineticWidget->view()->model() << endl ;
//	kineticWidget->view()->model()->connectToModel(items->model()) ; // TODO mysterious crashes here.
	qDebug() << "connected data model" << endl ;
//	kineticWidget->view()->model()->connectToPlot(plot) ;
	qDebug() << "connected data plot" << endl ;
// 	setTitleBarWidget(new specDockTitle) ;

	// TODO outsource... clean-up for log widget ... title for log dock window
	logWidget = new QDockWidget("Logs",parent) ;
	QWidget *logContent = new QWidget(logWidget) ;
	logWidget->setFloating(true) ;
	logs = new specLogView(logWidget) ;
	logs->setModel(new specLogModel(items->model(),logs));
	QToolBar *logToolbar = new QToolBar(logWidget) ;
	foreach(QAction* pointer, logs->actions())
		logToolbar->addAction(pointer) ;

	QVBoxLayout *logLayout = new QVBoxLayout ;
	logLayout->addWidget(logToolbar) ;
	logLayout->addWidget(logs) ;
	logContent->setLayout(logLayout);
	logWidget->setWidget(logContent);

	qDebug("creating actions, toolbars, connections") ;

	
	createActions() ;
	createToolbars();
	setConnections() ;
	
	splitter= new QSplitter ;
	splitter->setOrientation(Qt::Vertical) ;
	splitter->addWidget(plot) ;
	splitter->addWidget(items) ;
	splitter->setOpaqueResize(false) ;
	
	layout -> addWidget(toolbar) ;
	qDebug("adding undo toolbar") ;
	actions = new specActionLibrary(this) ;
	layout -> addWidget(actions->toolBar(items)) ;
	layout -> addWidget(actions->toolBar(plot)) ;
	layout -> addWidget(actions->toolBar(kineticWidget->view()));

	actions->addDragDropPartner(items->model()) ;
	actions->addDragDropPartner(kineticWidget->view()->model()) ;
	plot->setUndoPartner(actions) ;
	qDebug("added undo toolbar") ;
	layout -> addWidget(splitter)  ;
	layout -> setContentsMargins(0,0,0,0) ;
	content->setLayout(layout) ;
	setWidget(content) ;

	if (onDisk->exists())
	{
		qDebug() << "filepos:" << onDisk->pos() ;
		actions->read(in) ;
		items->read(in) ;
	}


	in.unsetDevice() ;
	onDisk->close() ;

	actions->addPlot(plot) ;
	actions->addPlot(kineticWidget->internalPlot()) ;
	qDebug() << "connected plots:" << plot << kineticWidget->internalPlot() ;
	kineticWidget->view()->assignDataView(items) ;
// TODO reconnect setFont() slot
}

void specPlotWidget::modified()
{
	QString currentTitle = windowTitle() ;
	if(currentTitle.right(1) != "*")
		currentTitle.append(" *") ;
	setWindowTitle(currentTitle) ;
	kineticWidget->setWindowTitle(QString("Kinetics of ").append(currentTitle)) ;
}

void specPlotWidget::createActions()
{
/*	importAction = new QAction(QIcon(":/fileimport.png"), tr("&Import"), this);
	importAction->setShortcut(tr("Ctrl+I"));
	importAction->setStatusTip(tr("Import a file"));
	connect(importAction, SIGNAL(triggered()), this, SLOT(modified())) ;
	connect(importAction, SIGNAL(triggered()), items, SLOT(importFile()));
	
	newItemAction = new QAction(QIcon(":/folder_new.png"), tr("&Neuer Eintrag"), this);
	importAction->setShortcut(tr("Ctrl+E"));
	importAction->setStatusTip(tr("Neuen Eintrag anlegen"));
	connect(newItemAction, SIGNAL(triggered()), this, SLOT(modified())) ;
	connect(newItemAction, SIGNAL(triggered()), items, SLOT(newItem()));
	
	deleteAction = new QAction(QIcon(":/item_delete.png"), tr("&Löschen"), this);
	deleteAction->setShortcut(tr("Entf"));
	deleteAction->setStatusTip(tr("Markierte Einträge löschen"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(modified())) ;
	connect(deleteAction, SIGNAL(triggered()), items, SLOT(deleteItems()));*/
	
	saveAction = new QAction(QIcon(":/filesave.png"), tr("&Save"), this);
	saveAction->setShortcut(tr("Strg+S"));
	saveAction->setStatusTip(tr("Save"));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	
	saveAsAction = new QAction(QIcon(":/filesaveas.png"), tr("&Save as..."), this);
	saveAsAction->setShortcut(tr("Strg+Shift+S"));
	saveAsAction->setStatusTip(tr("Save as..."));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	
/*	treeAction = new QAction(QIcon(":/tree.png"), tr("Baum aufbauen"), this) ;
	treeAction->setShortcut(tr("Ctrl+T"));
	treeAction->setStatusTip(tr("Elemente kategorisieren"));
	connect(treeAction, SIGNAL(triggered()), this, SLOT(modified())) ;
	connect(treeAction, SIGNAL(triggered()), items, SLOT(buildTree())) ;*/
	
	kineticsAction = kineticWidget->toggleViewAction() ;
	kineticsAction->setIcon(QIcon(":/kineticwindow.png")) ;
	kineticsAction->setText(tr("Kinetikfenster")) ;
	kineticsAction->setParent(this) ;
	kineticsAction->setShortcut(tr("Ctrl+K")) ;
	kineticsAction->setStatusTip(tr("Kinetikfenster anzeigen")) ;

	logAction = logWidget->toggleViewAction() ;
	logAction->setText(tr("Log-Fenster")) ;
	logAction->setParent(this) ;
	
	toKineticAction = new QAction(QIcon(":/toKinetic.png"), tr("Add to current kinetic"), this) ;
	toKineticAction ->setStatusTip(tr("To current kinetic")) ;
	connect(toKineticAction, SIGNAL(triggered()), this, SLOT(toKinetic())) ;
	
	fromKineticAction = new QAction(QIcon(":/fromKinetic.png"), tr("Select from current kinetic"), this) ;
	fromKineticAction ->setStatusTip(tr("Select items connected to current kinetic")) ;
	connect(fromKineticAction, SIGNAL(triggered()), this, SLOT(fromKinetic())) ;
}

void specPlotWidget::toKinetic()
{
//	kineticWidget->view()->addToCurrent(items->getSelection()) ;
}

void specPlotWidget::fromKinetic()
{
//	specKinetic *pointer = dynamic_cast<specKinetic*>((specModelItem*) kineticWidget->view()->currentIndex().internalPointer()) ;
//	if (pointer)
//	{
//		items->selectionModel()->select(QModelIndex(),QItemSelectionModel::Clear) ;
//		QItemSelection newSelection ;
//		QModelIndexList list = pointer->connectedData() ;
//		foreach(QModelIndex index, list)
//			newSelection.select(index,index.model()->index(index.row(),index.model()->columnCount(index.parent())-1,index.parent())) ;
//		items->selectionModel()->select(newSelection,QItemSelectionModel::Select) ;
//	}
}

void specPlotWidget::newKinetics()
{
	kineticWidget = new specKineticWidget(windowTitle(),this) ;
// 	QList<specModelItem*> list = items->currentlySelected() ;
// 	foreach(specModelItem* pointer, list)
// 		kineticWidget->addItem(pointer) ;
	kineticWidget->show() ;
}

// void specPlotWidget::zeroCorrection()
// {
// 	if (zeroCorrectionAction->isChecked())
// 	{
// ///		plot->picker()->setMoveMode(spec::newZero) ;
// // 		addZeroRangeAction->setEnabled(true) ;
// // 		individualZeroAction->setEnabled(true) ;
// 		scaleAction->setEnabled(false) ;
// 		offsetAction->setEnabled(false);
// 		offlineAction->setEnabled(false);
// 	}
// 	else
// 	{
// ///		plot->picker()->setMoveMode(spec::none) ;
// 		addZeroRangeAction->setEnabled(false) ;
// 		deleteZeroRangeAction->setEnabled(false) ;
// 		individualZeroAction->setEnabled(false) ;
// 		scaleAction->setEnabled(true) ; // TODO put actions into array
// 		offsetAction->setEnabled(true);
// 		offlineAction->setEnabled(true);
// 		scaleCurve() ;
// 		offlineCurve() ;
// 		offsetCurve() ;
// 	}
// 	plot->replot() ;
// }

void specPlotWidget::scaleCurve() // TODO set-up scale, offset, slope actions.
{
///	if (scaleAction->isChecked())
///		plot->picker()->enableMoveMode(spec::scale) ;
///	else
///		plot->picker()->disableMoveMode(spec::scale) ;
	plot->replot() ;
}

void specPlotWidget::offsetCurve()
{
// 	if (offsetAction->isChecked())
// 	{
///		plot->picker()->enableMoveMode(spec::shift) ;
// 		offlineAction->setEnabled(true) ;
///		if(offlineAction->isChecked())
///			plot->picker()->enableMoveMode(spec::slope) ;
// 	}
// 	else
// 	{
// 		offlineAction->setDisabled(true) ;
///		plot->picker()->disableMoveMode(spec::shift) ;
///		plot->picker()->disableMoveMode(spec::slope) ;
// 	}
// 	plot->replot() ;
}

void specPlotWidget::offlineCurve()
{
///	if (offlineAction->isChecked())
///		plot->picker()->enableMoveMode(spec::slope) ;
///	else
///		plot->picker()->disableMoveMode(spec::slope) ;
// 	plot->replot() ;
}

void specPlotWidget::createToolbars()
{
	toolbar = new QToolBar(tr("File"));
	toolbar-> setContentsMargins(0,0,0,0) ;
	toolbar-> setIconSize(QSize(20,20)) ;
	
	foreach(QAction* pointer, items->actions())
		toolbar->addAction(pointer) ;
	
	toolbar-> addAction(saveAction) ;
	toolbar-> addAction(saveAsAction) ;
// 	toolbar-> addAction(exportAction) ;
// 	toolbar-> addAction(scaleAction);
// 	toolbar-> addAction(offsetAction);
// 	toolbar-> addAction(offlineAction);
// 	toolbar-> addAction(zeroCorrectionAction) ;
// 	toolbar-> addAction(addZeroRangeAction);
// 	toolbar-> addAction(deleteZeroRangeAction);
// 	toolbar-> addAction(individualZeroAction);
	toolbar->addAction(logAction) ;
	toolbar-> addAction(kineticsAction) ;
	toolbar-> addAction(toKineticAction) ;
	toolbar-> addAction(fromKineticAction) ;
	foreach(QAction* action, plot->actions())
		toolbar->addAction(action) ;
// 	QMenu *testmenu = new QMenu("test") ;
// // 	testmenu->addAction(importAction) ;
// 	testmenu->setIcon(QIcon(":/folder_new.png")) ;
// // 	testmenu->addAction(scaleAction) ;
// 	toolbar->addAction(testmenu->menuAction()) ;
}

void specPlotWidget::closeEvent(QCloseEvent* event)
{
	qDebug("checking if user needs/wants to save") ;
	int needToSave = QMessageBox::No ;
	if (windowTitle().right(1) == "*") // Pruefe, ob Aenderungsindikator gesetzt.  Wenn ja, fragen ob speichern.
	{
		QMessageBox wantToSave ;
		wantToSave.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) ;
		wantToSave.setDefaultButton(QMessageBox::Cancel) ;
		wantToSave.setEscapeButton(QMessageBox::Cancel) ;
		wantToSave.setWindowTitle("Speichern?") ;
		wantToSave.setText(QString("Wollen Sie die Änderungen an ").
				append(windowTitle() != " *" ? windowTitle() : "der unbenannten Datei").
				append(" speichern?")) ;
		wantToSave.setIcon(QMessageBox::Warning) ;
		needToSave = wantToSave.exec() ;
	}
	else
	{
		event->accept() ;
		return ;
	}

	qDebug("performing action %d", needToSave) ;
	switch (needToSave)
	{
		case QMessageBox::Yes :
			if (! saveFile())
			{
				modified() ;
				break ;
			}
		case QMessageBox::No :
			kineticWidget->close() ;
			qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(kineticWidget) ;
			qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(this) ;
			qobject_cast<QMainWindow*>(parentWidget())->repaint() ;
			delete kineticWidget ;
			destroy(true,true) ;
			delete(this) ;
			event->accept() ;
			break ;
		default :
			event->ignore() ;
			break ;
	}
	qDebug("done closing dock") ;
}

void specPlotWidget::setFont()
{
	bool ok ;
	QFont newFont = QFontDialog::getFont(&ok,QFont("Times", 10),this) ;
	if (plot->itemList().size() && ok)
	{
		plot->setAxisFont(plot->itemList()[0]->xAxis(),newFont) ;
		plot->setAxisFont(plot->itemList()[0]->yAxis(),newFont) ;
	}
}

bool specPlotWidget::saveFile()
{
	changeFileName(onDisk->fileName() == "" || sender() == saveAsAction ? 
			QFileDialog::getSaveFileName(this,"Name?","","spec-Dateien (*.spec)") :
			onDisk->fileName()) ;
	if (onDisk->fileName() == "") return false ;
	onDisk->open(QFile::WriteOnly) ;
	QDataStream out(onDisk) ;
	out << *(kineticWidget) ;
	out << *(items->model()) ;
	qDebug() << "filepos:" << onDisk->pos() ;
	actions->write(out) ;
	items->write(out) ;
	out.unsetDevice() ;
	onDisk->close() ;
	return true ;
}

specPlotWidget::~specPlotWidget()
{
	kineticWidget->deleteLater();
	emit destroyed(this) ;
	destroy(true,true) ;
}

void specPlotWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
// 	if (previous.isValid())
// 		plot->picker()->removeItem((specModelItem*)previous.internalPointer()) ;
// 	plot->picker()->addItem((specModelItem*) current.internalPointer()) ;
}

void specPlotWidget::setConnections()
{
	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;
	connect(items->selectionModel(),SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&))) ;
//	connect(plot->picker(),SIGNAL(moved(specCanvasItem*)),kineticWidget->view()->model(),SLOT(conditionalUpdate(specCanvasItem*))) ;
// 	connect(plot->picker(),SIGNAL(rangesModified(QList<specRange*>*)),items,SLOT(newZeroRanges(QList<specRange*>*))) ;

	connect(items->model(),SIGNAL(modelAboutToBeReset()),items,SLOT(prepareReset())) ;
	connect(items->model(),SIGNAL(modelReset()),items,SLOT(resetDone())) ;
}

#include <QPainter>

void specPlotWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	foreach(QModelIndex index, deselected.indexes())
		if (!index.column() && index.isValid())
			((specModelItem*) index.internalPointer())->detach() ;
	
	foreach(QModelIndex index, selected.indexes())
		if (!index.column())
			((specModelItem*) index.internalPointer())->attach(plot) ;
	
// 	QTextStream cout(stdout, QIODevice::WriteOnly) ;
// 	qDebug("Selection changed  %d", selected.indexes().size()) ;
// 	if(selected.indexes().size() > 1)
// 	{
// 		QModelIndex index = selected.indexes()[1] ;
// 		QItemDelegate *delegate = new QItemDelegate(items) ;
// 		QPainter *painter = new QPainter ;
// 		items->setItemDelegateForRow(index.row(), delegate) ;
// 		painter->begin(items) ;
// 		painter->setBrush(QBrush(Qt::darkGreen)) ;
// 		delegate->drawBackground(painter,QStyleOptionViewItem(),index) ;
// 	}
	
	plot->replot() ;
}
