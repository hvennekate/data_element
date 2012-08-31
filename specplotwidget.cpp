#include "specplotwidget.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QMainWindow>
#include "specsplitter.h"
#include "specgenericmimeconverter.h"
#include "speclogtodataconverter.h"
#include "spectextmimeconverter.h"
#include "speckineticwidget.h"
#include "specactionlibrary.h"
#include "specdataview.h"
#include "specspectrumplot.h"
#include "speclogwidget.h"
#include "canvaspicker.h"

specPlotWidget::specPlotWidget(QWidget *parent)
	: QDockWidget(tr("untitled"),parent),
	  items(new specDataView(this)),
	  logWidget(new specLogWidget(parent)),
	  kineticWidget(new specKineticWidget(QString(),parent)),
	  content(new QWidget(this)),
	  layout(new QVBoxLayout(content)),
	  plot(new specSpectrumPlot(this)),
	  toolbar(new QToolBar(tr("File"),this)),
	  splitter(new specSplitter(Qt::Vertical,this)),
	  file(new QFile(this)),
	  saveAction(new QAction(QIcon::fromTheme("document-save"), tr("Save"), this)),
	  kineticsAction(kineticWidget->toggleViewAction()),
	  saveAsAction(new QAction(QIcon::fromTheme("document-save-as"), tr("Save as..."), this)),
	  logAction(logWidget->toggleViewAction()),
	  actions(new specActionLibrary(this))
{
	setWhatsThis(tr("Data Dock Window - This is the main window for managing data.  Right-clicking on it will change the arrangement of its contents from vertical to horizontal and back.\nSince this is a dock window, you may remove it from the application's main window and move it around within the main window.  To do so, \"grab\" it by its title bar and drag it to where you need it to be.\nThere are also a dock window for managing logs and \"meta-data\" (i.e. data based on processing the primary data) associated with this data dock window.  To show these, click the log or meta buttons (to the right of the save as button).\nTo start working, check out the help of the data list at the bottom (or right) of this window or of the list in the log dock window."));
	saveAsAction->setWhatsThis(tr("Save As... - This button will allow you to save your work asking you for a file name explicitly."));
	saveAction->setWhatsThis(tr("Save - Clicking this button will save your work and only ask for a file name if none has been specified so far."));

	undoAction = actions->undoAction(this) ;
	redoAction = actions->redoAction(this) ;
	undoAction->setToolTip(tr("Undo")) ;
	redoAction->setToolTip(tr("Redo")) ;
	undoAction->setToolTip(tr("Undo - By clicking this button you can revert changes.  To \"undo the undo\" click the redo button right next door.\nNote that all of your undo history will be saved along with your work and will be available again upon loading your file again."));
	redoAction->setToolTip(tr("Redo - Redoes what has been undone by clicking the undo button.\nNote that all of your undo history (including possible redos) will be saved along with your work and will be available again upon loading your file again."));

	items->setModel(new specModel(items));

	plot->setMinimumSize(100,100) ;
	plot->setView(items) ;
	plot->setUndoPartner(actions) ;

	new specGenericMimeConverter(items->model()) ;
	new specLogToDataConverter(items->model()) ;
	new specTextMimeConverter(items->model()) ;

	actions->addDragDropPartner(items->model()) ;
	actions->addPlot(plot) ;

	kineticWidget->view()->assignDataView(items) ;
	kineticWidget->view()->setUndoPartner(actions) ;
	kineticWidget->addToolbar(actions) ;
	kineticsAction->setIcon(QIcon(":/kineticwindow.png"));

	logWidget->addToolbar(actions) ;
	logAction->setIcon(QIcon(":/logs.png"));

	createToolbars();
	setConnections() ;

	splitter->addWidget(plot) ;
	splitter->addWidget(items) ;
	splitter->setOpaqueResize(false) ;

	layout -> addWidget(toolbar) ;
	QToolBar *itemToolBar = actions->toolBar(items) ;
	layout -> addWidget(itemToolBar) ;

	QToolBar *plotToolBar = actions->toolBar(plot) ;
	plotToolBar->addActions(plot->actions()) ;
	layout -> addWidget(plotToolBar) ;

	layout -> addWidget(splitter)  ;
	layout -> setContentsMargins(0,0,0,0) ;

	setWidget(content) ;
	svgModification(false) ;
}

void specPlotWidget::read(QString fileName)
{
	// Checking for existence ought to have been done at this point
	file->close(); // necessary?
	file->setFileName(fileName) ;
	if (!file->open(QFile::ReadOnly)) return ; // TODO warning message
	QDataStream in(file) ;
	// Basic layout of the file:
	quint64 check ;
	in >> check ;
	if (check != FILECHECKRANDOMNUMBER)
	{
		file->close();
		return ; // TODO warning
	}
	QByteArray fileContent = file->readAll() ;
	file->close();

	QDataStream inStream(fileContent) ;
	inStream >> *plot
		 >> *items
		 >> *logWidget
		 >> *kineticWidget
		 >> *actions ;
	changeFileName(fileName);
}

void specPlotWidget::modified()
{
	QString currentTitle = windowTitle() ;
	if(currentTitle.right(1) != "*")
		currentTitle.append(" *") ;
	setWindowTitle(currentTitle) ;
	kineticWidget->setWindowTitle(QString("Kinetics of ").append(currentTitle)) ;
	logWidget->setWindowTitle(QString("Logs of ").append(currentTitle)) ;
}

void specPlotWidget::createToolbars()
{
	toolbar-> setContentsMargins(0,0,0,0) ;
	toolbar-> setIconSize(QSize(20,20)) ;

	toolbar-> addAction(saveAction) ;
	toolbar-> addAction(saveAsAction) ;
	toolbar-> addSeparator() ;
	toolbar-> addAction(logAction) ;
	toolbar-> addAction(kineticsAction) ;
	toolbar-> addSeparator() ;
	toolbar-> addAction(undoAction) ;
	toolbar-> addAction(redoAction) ;

	toolbar->addSeparator() ;
	QAction *purgeUndoStack = new QAction(QIcon::fromTheme("user-trash"),tr("Clear undo"),this) ;
	toolbar-> addAction(purgeUndoStack) ;
	connect(purgeUndoStack,SIGNAL(triggered()),this,SLOT(purgeUndo())) ;
}

void specPlotWidget::purgeUndo()
{
	if (QMessageBox::Yes ==
			QMessageBox::question(this,
					      tr("Really Clear Undo?"),
					      tr("Do you really want to delete all undo and redo actions?  WARNING:  This cannot be undone."),
					      QMessageBox::Yes | QMessageBox::No,
					      QMessageBox::No))
		actions->purgeUndo() ;
}

void specPlotWidget::closeEvent(QCloseEvent* event)
{
	int needToSave = QMessageBox::No ;
	if (windowTitle().right(1) == "*") // Pruefe, ob Aenderungsindikator gesetzt.  Wenn ja, fragen ob speichern.
	{
		QMessageBox wantToSave ;
		wantToSave.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) ;
		wantToSave.setDefaultButton(QMessageBox::Cancel) ;
		wantToSave.setEscapeButton(QMessageBox::Cancel) ;
		wantToSave.setWindowTitle("Save data?") ;
		wantToSave.setText(QString("Do you want to save changes made to ").
				append(windowTitle() != " *" ? windowTitle() : "a new file").
				append(" ?")) ;
		wantToSave.setIcon(QMessageBox::Warning) ;
		needToSave = wantToSave.exec() ;
	}
	else
	{
		event->accept() ;
		delete this ;
		return ;
	}

	switch (needToSave)
	{
		case QMessageBox::Yes :
			if (! saveFile())
			{
				modified() ;
				event->ignore();
				break ;
			}
		case QMessageBox::No :
			delete(this) ;
			event->accept() ;
			break ;
		default :
			event->ignore() ;
			break ;
	}
}

bool specPlotWidget::saveFile()
{
	changeFileName(file->fileName() == "" || sender() == saveAsAction ?
			QFileDialog::getSaveFileName(this,"Name?","","spec-Dateien (*.spec)") :
			file->fileName()) ;
	if (file->fileName() == "") return false ;
	file->open(QFile::WriteOnly) ;
	QDataStream out(file) ;
	out << quint64(FILECHECKRANDOMNUMBER)
	    << *plot
	    << *items
	    << *logWidget
	    << *kineticWidget
	    << *actions ;
	file->close() ;
	return true ;
}

specPlotWidget::~specPlotWidget()
{
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(kineticWidget) ;
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(logWidget) ;
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(this) ;
	logWidget->deleteLater();
	kineticWidget->deleteLater();
}

void specPlotWidget::setConnections()
{
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;

	connect(actions,SIGNAL(stackChanged()), this, SLOT(modified())) ; // TODO check

	connect(kineticWidget->internalPlot(),SIGNAL(replotted()),plot,SLOT(replot())) ;
	connect(kineticWidget->internalPlot(), SIGNAL(metaRangeModified(specCanvasItem*,int,double,double)), kineticWidget->view(), SLOT(rangeModified(specCanvasItem*,int,double,double))) ;
	connect(plot, SIGNAL(metaRangeModified(specCanvasItem*,int,double,double)), kineticWidget->view(),SLOT(rangeModified(specCanvasItem*,int,double,double))) ;

	connect(items,SIGNAL(newUndoCommand(specUndoCommand*)), actions, SLOT(push(specUndoCommand*))) ;
	connect(kineticWidget->view(),SIGNAL(newUndoCommand(specUndoCommand*)), actions, SLOT(push(specUndoCommand*))) ;
	connect(plot->svgAction(),SIGNAL(toggled(bool)),this,SLOT(svgModification(bool))) ;
}

void specPlotWidget::svgModification(bool mod)
{
	if (mod) connect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;
	else disconnect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;

	plot->svgPicker()->highlightSelectable(mod) ;
}

void specPlotWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	foreach(QModelIndex index, deselected.indexes())
		if (!index.column() && index.isValid())
			((specModelItem*) index.internalPointer())->detach() ;
	
	foreach(QModelIndex index, selected.indexes())
		if (!index.column())
			((specModelItem*) index.internalPointer())->attach(plot) ;
	
	plot->replot() ;
}

specView* specPlotWidget::mainView()
{
	return items ;
}

void specPlotWidget::changeFileName(const QString& name)
{
	file->setFileName(name) ;
	setWindowTitle(QFileInfo(name).fileName()) ;
	kineticWidget->setWindowTitle(QString("Kinetics of ").append(QFileInfo(name).fileName())) ;
	logWidget->setWindowTitle(QString("Logs of ").append(QFileInfo(name).fileName())) ;
}
