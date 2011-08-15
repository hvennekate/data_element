#include "specappwindow.h"
#include <QFile>

specAppWindow::specAppWindow()
 : QMainWindow(), settings()
{
	setAnimated(false) ;
	setDockOptions(QMainWindow::AllowTabbedDocks) ;
	createActions();
	createMenus();
	createToolBars();
	restoreGeometry(settings.value("mainWindow/geometry").toByteArray()) ;
}


specAppWindow::~specAppWindow()
{
}

void specAppWindow::closeEvent(QCloseEvent* event)
{
	event->ignore() ;
	bool allClosed = true ;
	specPlotWidget* pointer ;
	foreach(QObject* dock, children())
		if(pointer = dynamic_cast<specPlotWidget*>(dock)) //
			allClosed = allClosed && pointer->close() ;
	event->setAccepted(allClosed) ;
	settings.setValue("mainWindow/geometry",saveGeometry()) ;
}

void specAppWindow::newFile()
{
	addDockWidget(Qt::LeftDockWidgetArea, new specPlotWidget("",this)) ;
}

void specAppWindow::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,"Name?","","spec-Dateien (*.spec)");
	if (fileName != "")
		addDockWidget(Qt::LeftDockWidgetArea, new specPlotWidget(fileName,this)) ;
}

void specAppWindow::createActions()
{
	newAction = new QAction(QIcon(":/filenew.png"), tr("&New"), this);
	newAction->setShortcut(tr("Ctrl+N"));
	newAction->setStatusTip(tr("Create a new file"));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
	
	openAction = new QAction(QIcon(":/fileopen.png"), tr("&Open"), this);
	openAction->setShortcut(tr("Ctrl+O"));
	openAction->setStatusTip(tr("Open an existing file"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
}

void specAppWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
}

void specAppWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAction);
	fileMenu->addAction(openAction);
}

