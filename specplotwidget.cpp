#include "specplotwidget.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QMainWindow>
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
	  splitter(new QSplitter(Qt::Vertical,this)),
	  file(new QFile(this)),
	  saveAction(new QAction(QIcon::fromTheme("document-save"), tr("Save"), this)),
	  kineticsAction(kineticWidget->toggleViewAction()),
	  saveAsAction(new QAction(QIcon::fromTheme("document-save-as"), tr("Save as..."), this)),
	  logAction(logWidget->toggleViewAction()),
	  actions(new specActionLibrary(this))
{
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
	layout -> addWidget(actions->toolBar(items)) ;
	layout -> addWidget(actions->toolBar(plot)) ;
	layout -> addWidget(splitter)  ;
	layout -> setContentsMargins(0,0,0,0) ;

	setWidget(content) ;
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
	toolbar-> addAction(actions->undoAction(this)) ;
	toolbar-> addAction(actions->redoAction(this)) ;
	toolbar-> addSeparator() ;

	foreach(QAction* action, plot->actions())
		toolbar->addAction(action) ;
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
	connect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;
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

void specPlotWidget::contextMenuEvent(QContextMenuEvent* event)
{
	if(splitter->geometry().contains(event->x(),event->y()))
	{
		if (splitter->orientation() == Qt::Horizontal)
			splitter->setOrientation(Qt::Vertical) ;
		else
			splitter->setOrientation(Qt::Horizontal) ;
	}
}
