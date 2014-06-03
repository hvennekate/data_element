#include "specappwindow.h"
#include <QFile>
#include "specshortcutdialog.h"
#include "names.h"

specAppWindow::specAppWindow()
	: QMainWindow(),
	  settings()
{
	setAnimated(false) ;
	setDockOptions(QMainWindow::AllowTabbedDocks) ;
	setDockNestingEnabled(true) ;
	createActions();
	createMenus();
	createToolBars();
	restoreGeometry(settings.value("mainWindow/geometry").toByteArray()) ;
	setCentralWidget(0);

	setObjectName("Application window") ;
	setWindowTitle("SpecDataElement");
	setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint);

	setWhatsThis("This is the main application window.  It can be used for docking data windows, log windows, and kinetic windows to it. to it.\nStart by creating a new file or by opening a saved file. Use the \"What's this?\" help from the title bar for further hints.");
	setMinimumSize(300, 300);
	if(restoreSessionAction->isChecked())
		foreach(QString fileName, settings.value("mainWindow/previousSessionFiles").toStringList())
		openFile(fileName) ;
}


specAppWindow::~specAppWindow()
{
}

void specAppWindow::closeEvent(QCloseEvent* event)
{
	event->ignore() ;
	QStringList openFileNames ;
	foreach(specPlotWidget * plotWidget, findChildren<specPlotWidget*>())
	{
		openFileNames << plotWidget->windowFilePath() ;
		if(!plotWidget->close()) return ;
	}
	settings.setValue("mainWindow/previousSessionFiles",
			  restoreSessionAction->isChecked() ?
			  openFileNames : QVariant()); // remove empty filenames?
	settings.setValue("mainWindow/geometry", saveGeometry()) ;
	settings.setValue("mainWindow/sessionRestoration", restoreSessionAction->isChecked()) ;
	event->accept();
}

void specAppWindow::newFile()
{
	addDock(new specPlotWidget(this)) ;
}

void specAppWindow::addDock(specPlotWidget* newDock)
{
	specPlotWidget* inAreaWidget = 0 ;
	foreach(specPlotWidget * widget, findChildren<specPlotWidget*>())
	if(dockWidgetArea(widget) == Qt::LeftDockWidgetArea)
		inAreaWidget = widget ;
	addDockWidget(Qt::LeftDockWidgetArea, newDock) ;
	if(inAreaWidget)
		tabifyDockWidget(inAreaWidget, newDock);
	specShortcutDialog *shortcutDialog = findChild<specShortcutDialog*>() ;
	if (shortcutDialog)
		shortcutDialog->assignShortcuts();
}

void specAppWindow::openFile()
{
	openFile(QFileDialog::getOpenFileName(this, "Name?", "", "spec-Dateien (*.spec)"));
}

void specAppWindow::openFile(const QString& fileName)
{
	if(fileName != "" && QFile::exists(fileName))
	{
		specPlotWidget* newWidget = new specPlotWidget(this) ;
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
	restoreSessionAction->setChecked(settings.value("mainWindow/sessionRestoration", false).toBool()) ;

	shortCutAction = new QAction(QIcon::fromTheme("input-keyboard"), tr("Shortcuts..."), this) ;
	connect(shortCutAction, SIGNAL(triggered()), this, SLOT(editShortcuts())) ;

	whatsThisAction = new QAction(QIcon::fromTheme("help-contextual"), tr("&What's this"), this) ;
	whatsThisAction->setStatusTip(tr("What's this?")) ;
	connect(whatsThisAction, SIGNAL(triggered()), this, SLOT(whatsThisMode())) ;
}

void specAppWindow::editShortcuts()
{
	specShortcutDialog *shortcutDialog = findChild<specShortcutDialog*>() ;
	if (shortcutDialog) shortcutDialog->exec();
}

void specAppWindow::whatsThisMode()
{
	QWhatsThis::enterWhatsThisMode() ;
}

void specAppWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File toolbar"));
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(shortCutAction) ;
	fileToolBar->addAction(restoreSessionAction) ;
	fileToolBar->addAction(whatsThisAction) ;
}

void specAppWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAction);
	fileMenu->addAction(openAction);
	fileMenu->addAction(shortCutAction) ;
	fileMenu->addAction(restoreSessionAction) ;

	helpMenu = menuBar()->addMenu(tr("&Help")) ;
	helpMenu->addAction(whatsThisAction) ;
	QAction* aboutQtAction = new QAction(tr("About &Qt..."), helpMenu) ;
	helpMenu->addAction(aboutQtAction) ;
	QAction* aboutAction = new QAction(tr("&About..."), helpMenu) ;
	helpMenu->addAction(aboutAction) ;
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt())) ;
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about())) ;
}

void specAppWindow::about()
{
	QMessageBox::about(this, tr("About SpecDataElement"),
			   tr("This is a simple program for efficiently managing two dimensional data and keeping track of experimental logs.\n\n"
			      "It makes use of the following libraries:\n"
			      "- Qt (qt.digia.com)\n"
			      "- Qwt (qwt.sourceforge.net)\n"
			      "- muParser (muparser.sourceforge.net)\n"
			      "- lmfit (joachimwuttke.de/lmfit)"
			      "- GSL (www.gnu.org/software/gsl)")
			   + tr("\n\nVersion ID is: ") + QString(STRINGIFYMACRO(GITSHA1HASH))) ;
}

