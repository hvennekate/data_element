#include "specplotwidget.h"

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
	: specDockWidget(tr("Data"), parent, false),
	  actions(new specActionLibrary(this)),
	  items(new specDataView(this)),
	  kineticWidget(new specKineticWidget(parent)),
	  logWidget(new specLogWidget(parent)),
	  plot(new specSpectrumPlot(this)),
	  saveAction(new QAction(QIcon::fromTheme("document-save"), tr("Save"), this)),
	  kineticsAction(kineticWidget->toggleViewAction()),
	  saveAsAction(new QAction(QIcon::fromTheme("document-save-as"), tr("Save as..."), this)),
	  logAction(logWidget->toggleViewAction()),
	  undoViewWidget(actions->undoWidget())
{
	setVisible(true) ;
	items->setModel(new specModel(items)); // TODO redundant!  durchschleifen!
	plot->setView(items) ;

	new specGenericMimeConverter(items->model()) ;
	new specLogToDataConverter(items->model()) ;
	new specMimeFileImporter(items->model()) ;
	new specTextMimeConverter(items->model()) ;

	kineticWidget->view()->assignDataView(items) ;

	kineticsAction->setIcon(QIcon(":/kineticwindow.png"));
	logAction->setIcon(QIcon(":/logs.png"));

	setConnections() ;
	createWhatsThis();

	changeFileName(tr("untitled")) ;
	svgModification(false) ;
	kineticWidget->setupWindow(actions);
	logWidget->setupWindow(actions);
	setupWindow(actions);
	subDocks << logWidget << kineticWidget << undoViewWidget ;

	changeEvent(new QEvent(QEvent::WindowTitleChange));
	setObjectName("mainWidget");
}

void specPlotWidget::read(QString fileName)
{
	// Checking for existence ought to have been done at this point
	QFile file(fileName) ;
	if (!file.open(QFile::ReadOnly))
	{
		QMessageBox::critical(0,tr("Error opening"),
					  tr("Cannot open file ")
					  + fileName
					  + tr(" for reading.")) ;
		return ;
	}

	// Basic layout of the file:
	quint64 check ;
	QDataStream inStream(&file) ;
	inStream >> check ;
	if (check != FILECHECKRANDOMNUMBER && check != FILECHECKCOMPRESSNUMBER)
	{
		QMessageBox::critical(0,tr("File error"),
					  tr("File ")
					  + fileName
					  + tr("does not seem to have the right format.")) ;
		return ;
	}

	QByteArray fileContent = file.readAll() ;
	file.close();
	bzipIODevice *zipDevice = 0 ;
	QBuffer buffer(&fileContent) ; // does not take ownership of byteArray
	if (FILECHECKCOMPRESSNUMBER == check)
	{
		zipDevice = new bzipIODevice(&buffer) ; // takes ownership of buffer
		zipDevice->open(bzipIODevice::ReadOnly) ;
		inStream.setDevice(zipDevice);
	}
	else
	{
		buffer.open(QBuffer::ReadOnly) ;
		inStream.setDevice(&buffer) ;
	}

	QProgressDialog progress ;
	progress.setMaximum(30) ;
	progress.setMinimumDuration(300);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(0);
	progress.setWindowTitle(tr("Opening ") + file.fileName());
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
	qint8 visibility = 0 ;
	inStream >> visibility ;
	if (zipDevice) zipDevice->releaseDevice(); // release ownership of buffer
	delete zipDevice ;

	changeFileName(fileName);
	qint8 i = 1 ;
	foreach (specDockWidget* subDock, subDocks)
	{
		subDock->setVisible(visibility & i);
		i *= 2 ;
	}
}

QToolBar* specPlotWidget::createToolbar()
{
	QToolBar *toolbar = new QToolBar(tr("Main"), this) ;
	toolbar-> addAction(saveAction) ;
	toolbar-> addAction(saveAsAction) ;
	toolbar-> addSeparator() ;
	toolbar-> addAction(logAction) ;
	toolbar-> addAction(kineticsAction) ;
	toolbar-> addAction(undoViewWidget->toggleViewAction()) ;
	return toolbar ;
}

void specPlotWidget::closeEvent(QCloseEvent* event)
{
	if (!isWindowModified()) // Pruefe, ob Aenderungsindikator gesetzt.  Wenn ja, fragen ob speichern.
	{
		event->accept() ;
		deleteLater();
		return ;
	}

	QMessageBox wantToSave ;
	wantToSave.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) ;
	wantToSave.setDefaultButton(QMessageBox::Cancel) ;
	wantToSave.setEscapeButton(QMessageBox::Cancel) ;
	wantToSave.setWindowTitle("Save data?") ;
	wantToSave.setText(QString("Do you want to save changes made to ")
			   + windowFilePath() + tr("?") ) ;
	wantToSave.setIcon(QMessageBox::Warning) ;
	int wantToSaveResult = wantToSave.exec() ;

	event->ignore();
	if (QMessageBox::No == wantToSaveResult ||
			(QMessageBox::Yes == wantToSaveResult
			 && saveFile()))
	{
		event->accept();
		deleteLater();
	}
}

bool specPlotWidget::saveFile()
{
	QFile file(windowFilePath() == tr("untitled") || sender() == saveAsAction ?
			   QFileDialog::getSaveFileName(this,"Name?","","spec-Dateien (*.spec)") :
			   windowFilePath()) ;
	if (file.fileName().isEmpty()) return false ;

	QBuffer buffer ;
	buffer.open(QBuffer::WriteOnly) ;
	QDataStream out(&buffer) ;
	out << quint64(FILECHECKCOMPRESSNUMBER) ;
	out.setDevice(0); // safety
	bzipIODevice zipDevice(&buffer) ;
	zipDevice.open(bzipIODevice::WriteOnly) ;
	QDataStream zipOut(&zipDevice) ;

	QProgressDialog progress ;
	progress.setCancelButton(0);
	progress.setMinimumDuration(300);
	progress.setWindowModality(Qt::WindowModal);
	progress.setWindowTitle(tr("Saving ") + file.fileName());
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
	qint8 visibility = 0, i = 1 ;
	foreach(specDockWidget *subDock, subDocks)
	{
		visibility += i * subDock->isVisible() ;
		i *= 2 ;
	}
	zipOut << visibility ;
	zipDevice.close() ;
	zipDevice.releaseDevice() ;
	buffer.close();
	if (!file.open(QFile::WriteOnly))
	{
		QMessageBox::critical(0, tr("Write error"), tr("Could not open file ")
					  + file.fileName()
					  + tr("for writing.")) ;
		return false ;
	}
	if (file.write(buffer.data()) == -1)
	{
		QMessageBox::critical(0, tr("Severe error!"), tr("Error writing file!  Data could not be saved!")) ;
		return false ;
	}
	file.close();
	changeFileName(file.fileName());
	return true ;
}

specPlotWidget::~specPlotWidget()
{
	foreach (specDockWidget *subDock, subDocks)
		delete subDock ; // No deleteLater here, otherwise we get in trouble with the mainPlot autodeleting/detaching metaRanges
	qobject_cast<QMainWindow*>(parentWidget())->removeDockWidget(this) ;
}

void specPlotWidget::setConnections()
{
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(kineticWidget->internalPlot(),SIGNAL(replotted()),plot,SLOT(replot())) ;
	connect(kineticWidget->internalPlot(), SIGNAL(metaRangeModified(specCanvasItem*,int,double,double)), kineticWidget->view(), SLOT(rangeModified(specCanvasItem*,int,double,double))) ;
	connect(plot, SIGNAL(metaRangeModified(specCanvasItem*,int,double,double)), kineticWidget->view(),SLOT(rangeModified(specCanvasItem*,int,double,double))) ;
	connect(plot->svgAction(),SIGNAL(toggled(bool)),this,SLOT(svgModification(bool))) ;
	connect(actions, SIGNAL(stackModified(bool)), this, SLOT(setWindowModified(bool))) ;
}

void specPlotWidget::svgModification(bool mod)
{
	if (mod) connect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;
	else disconnect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;

	plot->svgPicker()->highlightSelectable(mod) ;
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

void specPlotWidget::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::ModifiedChange)
		foreach(specDockWidget* subDock, subDocks)
			subDock->setWindowModified(isWindowModified());
	if (event->type() == QEvent::WindowTitleChange)
	{
		foreach(specDockWidget* subDock, subDocks)
		{
			subDock->setWindowFilePath(windowFilePath()) ;
			subDock->setWindowTitle(QString()) ; // just to invoke the event
		}
	}
	specDockWidget::changeEvent(event) ;
}

QList<QWidget*> specPlotWidget::mainWidgets() const
{
	return QList<QWidget*>() << items << plot ;
}

void specPlotWidget::createWhatsThis()
{
	setWhatsThis(tr("Data Dock Window - This is the main window for managing data.  Right-clicking on it will change the arrangement of its contents from vertical to horizontal and back.\nSince this is a dock window, you may remove it from the application's main window and move it around within the main window.  To do so, \"grab\" it by its title bar and drag it to where you need it to be.\nThere are also a dock window for managing logs and \"meta-data\" (i.e. data based on processing the primary data) associated with this data dock window.  To show these, click the log or meta buttons (to the right of the save as button).\nTo start working, check out the help of the data list at the bottom (or right) of this window or of the list in the log dock window."));
	saveAsAction->setWhatsThis(tr("Save As... - This button will allow you to save your work asking you for a file name explicitly."));
	saveAction->setWhatsThis(tr("Save - Clicking this button will save your work and only ask for a file name if none has been specified so far."));
	kineticsAction->setWhatsThis(tr("Shows and hides the meta widget."));
	logAction->setWhatsThis(tr("Shows and hides the log widget."));
}
