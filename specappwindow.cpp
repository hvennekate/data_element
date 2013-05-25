#include "specappwindow.h"
#include <QFile>

specAppWindow::specAppWindow()
	: QMainWindow(), settings()
{
	setAnimated(false) ;
	setDockOptions(QMainWindow::AllowTabbedDocks) ;
	setDockNestingEnabled(true) ;
	createActions();
	createMenus();
	createToolBars();
	restoreGeometry(settings.value("mainWindow/geometry").toByteArray()) ;
	setCentralWidget(0);

	setObjectName("specDataElementApplicationWindow") ;
	setWindowTitle("SpecDataElement");
	setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint);

	setWhatsThis("This is the main application window.  It can be used for docking data windows, log windows, and kinetic windows to it. to it.\nStart by creating a new file or by opening a saved file. Use the \"What's this?\" help from the title bar for further hints.");
	setMinimumSize(300,300);
	if (restoreSessionAction->isChecked())
		foreach(QString fileName, settings.value("mainWindow/previousSessionFiles").toStringList())
			openFile(fileName) ;
}


specAppWindow::~specAppWindow()
{
}

QStringList specAppWindow::openFileNames() const
{
    QStringList result ;
    foreach(specPlotWidget* plotWidget, findChildren<specPlotWidget*>())
        result << plotWidget->fileName() ;
    result.removeAll(QString()) ;
    return result ;
}

void specAppWindow::closeEvent(QCloseEvent* event)
{
	event->ignore() ;
	bool allClosed = true ;
	while(!docks.isEmpty())
	{
		if (!docks.first()->close())
		{
			return ;
		}
	}
	settings.setValue("mainWindow/previousSessionFiles",
			  restoreSessionAction->isChecked() ?
				  openFileNames: QVariant()); // remove empty filenames?
	event->setAccepted(allClosed) ;
	settings.setValue("mainWindow/geometry",saveGeometry()) ;
	settings.setValue("mainWindow/sessionRestoration", restoreSessionAction->isChecked()) ;
}

void specAppWindow::newFile()
{
	addDock(new specPlotWidget(this)) ;
}

void specAppWindow::removeDock()
{
	docks.removeAll((specPlotWidget*) sender()) ;
}

void specAppWindow::addDock(specPlotWidget *newDock)
{
	docks << newDock ;
	connect(newDock,SIGNAL(destroyed()),this,SLOT(removeDock())) ;
    foreach(specPlotWidget* widget, docks)
	specPlotWidget *inAreaWidget = 0 ;
		if (dockWidgetArea(widget) == Qt::LeftDockWidgetArea)
			inAreaWidget = widget ;
	addDockWidget(Qt::LeftDockWidgetArea, newDock) ;
	if (inAreaWidget)
		tabifyDockWidget(inAreaWidget, newDock);
}

void specAppWindow::openFile()
{
	openFile(QFileDialog::getOpenFileName(this,"Name?","","spec-Dateien (*.spec)"));
}

void specAppWindow::openFile(const QString &fileName)
{
	if (fileName != "" && QFile::exists(fileName))
	{
		specPlotWidget *newWidget = new specPlotWidget(this) ;
		newWidget->read(fileName) ;
		addDock(newWidget) ;
	}
}

void specAppWindow::createActions()
{
	newAction = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
	newAction->setShortcut(tr("Ctrl+N"));
	newAction->setStatusTip(tr("Create a new file"));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
	
	openAction = new QAction(QIcon::fromTheme("document-open"), tr("&Open"), this);
	openAction->setShortcut(tr("Ctrl+O"));
	openAction->setStatusTip(tr("Open an existing file"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

	restoreSessionAction = new QAction(QIcon::fromTheme("document-revert"), tr("Session &restoration"), this) ;
	restoreSessionAction->setStatusTip(tr("Toggle saving of session information")) ;
	restoreSessionAction->setCheckable(true) ;
	restoreSessionAction->setChecked(settings.value("mainWindow/sessionRestoration",false).toBool()) ;


	whatsThisAction = new QAction(QIcon::fromTheme("help-contextual"), tr("&What's this"), this) ;
	whatsThisAction->setStatusTip(tr("What's this?")) ;
	connect(whatsThisAction, SIGNAL(triggered()), this, SLOT(whatsThisMode())) ;
}

void specAppWindow::whatsThisMode()
{
	QWhatsThis::enterWhatsThisMode() ;
}

void specAppWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(restoreSessionAction) ;
	fileToolBar->addAction(whatsThisAction) ;
}

void specAppWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAction);
	fileMenu->addAction(openAction);
	fileMenu->addAction(restoreSessionAction) ;

	helpMenu = menuBar()->addMenu(tr("&Help")) ;
	helpMenu->addAction(whatsThisAction) ;
	QAction *aboutQtAction = new QAction(tr("About &Qt..."),helpMenu) ;
	helpMenu->addAction(aboutQtAction) ;
	QAction *aboutAction = new QAction(tr("&About..."),helpMenu) ;
	helpMenu->addAction(aboutAction) ;
	connect(aboutQtAction, SIGNAL(triggered()),qApp,SLOT(aboutQt())) ;
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about())) ;
}

void specAppWindow::about()
{
	QMessageBox::about(this, tr("About SpecDataElement"),
			   tr("This is a simple program for efficiently managing two dimensional data and keeping track of experimental logs.\n\n"
			      "It makes use of the following libraries:\n"
			      "- Qt 4.8 (qt.digia.com)\n"
			      "- Qwt 6 (qwt.sourceforge.net)\n"
			      "- muParser 2.2 (muparser.sourceforge.net)\n"
			      "- lmfit 3.3 (joachimwuttke.de/lmfit)")) ;
}

