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
#include "specmimefileimporter.h"
#include "speckineticwidget.h"
#include "specactionlibrary.h"
#include "specdataview.h"
#include "specspectrumplot.h"
#include "speclogwidget.h"
#include "canvaspicker.h"
#include <QUndoView>
#include <QCloseEvent>
#include "bzipiodevice.h"
#include <QBuffer>
#include <QProgressDialog>
#include <QLabel>

specPlotWidget::specPlotWidget(QWidget *parent)
	: specDockWidget(tr("Data"),parent),
	  items(new specDataView(this)),
	  logWidget(new specLogWidget(parent)),
	  undoViewWidget(new specDockWidget(tr("History"),parent)),
	  kineticWidget(new specKineticWidget(parent)),
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
	  undoViewAction(undoViewWidget->toggleViewAction()),
	  purgeUndoAction(new QAction(QIcon::fromTheme("user-trash"),tr("Clear history"),this)),
	  actions(new specActionLibrary(this))
{
	undoAction = actions->undoAction(this) ;
	redoAction = actions->redoAction(this) ;

	setWhatsThis(tr("Data Dock Window - This is the main window for managing data.  Right-clicking on it will change the arrangement of its contents from vertical to horizontal and back.\nSince this is a dock window, you may remove it from the application's main window and move it around within the main window.  To do so, \"grab\" it by its title bar and drag it to where you need it to be.\nThere are also a dock window for managing logs and \"meta-data\" (i.e. data based on processing the primary data) associated with this data dock window.  To show these, click the log or meta buttons (to the right of the save as button).\nTo start working, check out the help of the data list at the bottom (or right) of this window or of the list in the log dock window."));
	saveAsAction->setWhatsThis(tr("Save As... - This button will allow you to save your work asking you for a file name explicitly."));
	saveAction->setWhatsThis(tr("Save - Clicking this button will save your work and only ask for a file name if none has been specified so far."));
	purgeUndoAction->setWhatsThis(tr("Deletes the complete undo history -- use with care and don't point this at humans."));
	undoAction->setToolTip(tr("Undo")) ;
	redoAction->setToolTip(tr("Redo")) ;
	undoAction->setWhatsThis(tr("Undo - By clicking this button you can revert changes.  To \"undo the undo\" click the redo button right next door.\nNote that all of your undo history will be saved along with your work and will be available again upon loading your file again."));
	redoAction->setWhatsThis(tr("Redo - Redoes what has been undone by clicking the undo button.\nNote that all of your undo history (including possible redos) will be saved along with your work and will be available again upon loading your file again."));

	undoViewWidget->setWhatsThis("Undo history.  Click on any command to forward/rewind to that particular state.");
	undoViewWidget->setWidget(actions->undoView()) ;
	undoViewWidget->setFloating(true) ;
	undoViewAction->setIcon(QIcon(":/undoView.png")) ;
	undoViewAction->setWhatsThis(tr("Shows and hides the undo history."));

	items->setModel(new specModel(items)); // TODO redundant!  durchschleifen!
	items->setActionLibrary(actions) ;

	plot->setMinimumSize(100,100) ;
	plot->setView(items) ;
	plot->setUndoPartner(actions) ;

	new specGenericMimeConverter(items->model()) ;
	new specLogToDataConverter(items->model()) ;
	new specMimeFileImporter(items->model()) ;
	new specTextMimeConverter(items->model()) ;

	actions->addDragDropPartner(items->model()) ;
	actions->addPlot(plot) ;

	kineticWidget->view()->assignDataView(items) ;
	kineticWidget->view()->setActionLibrary(actions) ; // TODO redundant!  durchschleifen!
	kineticWidget->addToolbar(actions) ;
	kineticsAction->setIcon(QIcon(":/kineticwindow.png"));
	kineticsAction->setWhatsThis(tr("Shows and hides the meta widget."));

	logWidget->addToolbar(actions) ;
	logAction->setIcon(QIcon(":/logs.png"));
	logAction->setWhatsThis(tr("Shows and hides the log widget."));
	logWidget->view()->setActionLibrary(actions) ;

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
	layout ->setSpacing(0);

	setWidget(content) ;
	svgModification(false) ;
}

void specPlotWidget::read(QString fileName)
{
	// Checking for existence ought to have been done at this point
	file->close(); // necessary?
	QString oldFileName(this->fileName()) ;
	changeFileName(fileName) ;
	if (!file->open(QFile::ReadOnly))
	{
		QMessageBox::critical(0,tr("Read error"), tr("Cannot opten file ") + fileName + tr(" for reading.")) ;
		changeFileName(oldFileName);
		return ;
	}
	QDataStream in(file) ;
	// Basic layout of the file:
	quint64 check ;
	in >> check ;
	if (check != FILECHECKRANDOMNUMBER && check != FILECHECKCOMPRESSNUMBER)
	{
		file->close();
		QMessageBox::critical(0,tr("Error opening"), tr("File ") + fileName + tr("does not seem to have the right format.")) ;
		changeFileName(oldFileName);
		return ;
	}
	QByteArray fileContent = file->readAll() ;
	file->close();

	QDataStream inStream(fileContent) ;
	bzipIODevice *zipDevice = 0 ;
	QBuffer buffer(&fileContent) ; // does not take ownership of byteArray
	if (FILECHECKCOMPRESSNUMBER == check)
	{
		zipDevice = new bzipIODevice(&buffer) ; // takes ownership of buffer
		zipDevice->open(bzipIODevice::ReadOnly) ;
		inStream.setDevice(zipDevice);
	}
//	if (!inStream.device()) inStream.setDevice(&buffer) ;

    QProgressDialog progress ;
    progress.setMaximum(30) ;
    progress.setMinimumDuration(300);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(0);
    progress.setWindowTitle(tr("Opening ") + file->fileName());
    progress.setLabel(new QLabel(tr("Reading plot"))) ;
    progress.setValue(0);
    inStream >> *plot ;
    progress.setLabel(new QLabel(tr("Reading data items"))) ;
    progress.setValue(1);
    inStream >> *items ;
    progress.setLabel(new QLabel(tr("Reading log data"))) ;
    progress.setValue(2);
    inStream >> *logWidget ;
    progress.setLabel(new QLabel(tr("Reading meta items"))) ;
    progress.setValue(3) ;
    inStream >> *kineticWidget ;
    progress.setLabel(new QLabel(tr("Reading undo history")));
    progress.setValue(4);
    actions->setProgressDialog(&progress);
    inStream >> *actions ;
    progress.setValue(5);
    qint8 vint = 0 ;
    inStream >> vint ;
    spec::subDockVisibilityFlags visibility(vint) ;
	if (zipDevice) zipDevice->releaseDevice(); // release ownership of buffer
	delete zipDevice ;

	changeFileName(fileName);
    logWidget->setVisible(visibility & spec::logVisible);
    kineticWidget->setVisible(visibility & spec::metaVisible) ;
    undoViewWidget->setVisible(visibility & spec::undoVisible) ;
}

void specPlotWidget::unmodified(bool unmod)
{
	QString currentTitle = windowTitle() ;
	if(currentTitle.right(1) != "*" && !unmod)
		currentTitle.append(" *") ;
	if(currentTitle.right(1) == "*" && unmod)
		currentTitle.remove(currentTitle.size()-2,2) ;
	setWindowTitle(currentTitle) ;
	kineticWidget->setWindowTitle(QString("Kinetics of ").append(currentTitle)) ;
	logWidget->setWindowTitle(QString("Logs of ").append(currentTitle)) ;
	undoViewWidget->setWindowTitle(QString("History of ").append(currentTitle)) ;
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
	toolbar-> addAction(undoViewAction) ;
	toolbar-> addSeparator() ;
	toolbar-> addAction(purgeUndoAction) ;
}

void specPlotWidget::purgeUndo()
{
	if (QMessageBox::Yes ==
			QMessageBox::question(this,
					      tr("Really Clear History?"),
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
				unmodified(true) ;
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
	QString filename(file->fileName() == "" || sender() == saveAsAction ?
			QFileDialog::getSaveFileName(this,"Name?","","spec-Dateien (*.spec)") :
			file->fileName()),
		oldFileName = this->fileName() ;
	if (filename == "") return false ;
	changeFileName(filename) ;
	if (!file->open(QFile::WriteOnly))
	{
		QMessageBox::critical(0, tr("Write error"), tr("Could not open file ")
				      + file->fileName()
				      + tr("for writing.")) ;
		changeFileName(oldFileName) ;
		unmodified(false);
		return false ;
	}

	QDataStream out(file) ;
	out << quint64(FILECHECKCOMPRESSNUMBER) ;
//	QBuffer *outBuffer = new QBuffer;
	bzipIODevice zipDevice(file) ;
	zipDevice.open(bzipIODevice::WriteOnly) ;
	QDataStream zipOut(&zipDevice) ;

    QProgressDialog progress ;
    progress.setCancelButton(0);
    progress.setMinimumDuration(300);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(tr("Saving ") + file->fileName());
    progress.setMaximum(30);
    progress.setLabel(new QLabel(tr("Saving plot"))) ;
    progress.setValue(0);
    zipOut << *plot ;
    progress.setLabel(new QLabel(tr("Saving data items"))) ;
    progress.setValue(1);
    zipOut<< *items ;
    progress.setLabel(new QLabel(tr("Saving log data"))) ;
    progress.setValue(2);
    zipOut<< *logWidget ;
    progress.setLabel(new QLabel(tr("Saving meta data"))) ;
    progress.setValue(3) ;
    zipOut << *kineticWidget ;
    progress.setLabel(new QLabel(tr("Saving undo history"))) ;
    progress.setValue(4);
    actions->setProgressDialog(&progress) ;
    zipOut<< *actions ;
    progress.setValue(progress.maximum());
    spec::subDockVisibilityFlags visibility = spec::noneVisible ;
    if (kineticWidget->isVisible()) visibility |= spec::metaVisible ;
    if (undoViewWidget->isVisible()) visibility |= spec::undoVisible ;
    if (logWidget->isVisible()) visibility |= spec::logVisible ;
    zipOut << (qint8) visibility ;
	zipDevice.close() ;
    zipDevice.releaseDevice() ;
    file->close();
	return true ;
}

specPlotWidget::~specPlotWidget()
{
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(kineticWidget) ;
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(logWidget) ;
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(undoViewWidget) ;
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(this) ;
	logWidget->deleteLater();
	kineticWidget->deleteLater();
}

void specPlotWidget::setConnections()
{
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(purgeUndoAction,SIGNAL(triggered()),this,SLOT(purgeUndo())) ;
	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;
	connect(actions,SIGNAL(stackClean(bool)), this, SLOT(unmodified(bool))) ; // TODO check
	connect(kineticWidget->internalPlot(),SIGNAL(replotted()),plot,SLOT(replot())) ;
	connect(kineticWidget->internalPlot(), SIGNAL(metaRangeModified(specCanvasItem*,int,double,double)), kineticWidget->view(), SLOT(rangeModified(specCanvasItem*,int,double,double))) ;
	connect(plot, SIGNAL(metaRangeModified(specCanvasItem*,int,double,double)), kineticWidget->view(),SLOT(rangeModified(specCanvasItem*,int,double,double))) ;

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
	setWindowFilePath(name);
	event(new QEvent(QEvent::WindowTitleChange)) ;
}

QString specPlotWidget::fileName() const
{
    if (!file) return "" ;
    return file->fileName() ;
}
