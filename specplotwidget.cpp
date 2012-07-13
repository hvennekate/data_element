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
#include "speclogtodataconverter.h"


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

specPlotWidget::specPlotWidget(QWidget *parent)
	: QDockWidget(tr("untitled"),parent),
	  items(new specDataView(new specModel(items), this)),
	  logWidget(new specLogWidget(items->model(), parent)),
	  kineticWidget(new specKineticWidget(QString(),this)),
	  content(new QWidget),
	  layout(new QVBoxLayout),
	  plot(new specSpectrumPlot),
	  toolbar(new QToolBar(tr("File"),this)),
	  splitter(new QSplitter(Qt::Vertical,this)),
	  file(new QFile(this)),
	  saveAction(new QAction(QIcon::fromTheme("document-save"), tr("Save"), this)),
	  kineticsAction(kineticWidget->toggleViewAction()),
	  logAction(logWidget->toggleViewAction()),
	  saveAsAction(new QAction(QIcon::fromTheme("document-save-as"), tr("Save as..."), this)),
	  printAction(new QAction(QIcon::fromTheme("document-print"), tr("Print..."), this))
{
	plot->setMinimumSize(100,100) ;
	plot->setView(items) ;
	plot->setUndoPartner(actions) ;

	items->model()->mimeConverters[QString("application/spec.spectral.item")] = new specMimeConverter ;
	items->model()->mimeConverters[QString("application/spec.log.item")] = new specLogToDataConverter ;

	actions->addDragDropPartner(items->model()) ;
	actions->addPlot(plot) ;

	kineticWidget->view()->assignDataView(items) ;
	kineticWidget->addToolbar(actions) ;

	logWidget->addToolbar(actions) ;

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

	content->setLayout(layout) ;
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

	QDataStream in(fileContent) ;
	if (in.toNext(spec::mainWidget))
		in >> *plot
		   >> *items
		   >> *logWidget
		   >> *kineticWidget
		   >> *actions ;
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
	toolbar-> addAction(printAction) ;
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
			delete kineticWidget ; // TODO destructor!?
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
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;
	connect(items->selectionModel(),SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&))) ;

	connect(items->model(),SIGNAL(modelAboutToBeReset()),items,SLOT(prepareReset())) ;
	connect(items->model(),SIGNAL(modelReset()),items,SLOT(resetDone())) ;
	connect(actions,SIGNAL(stackChanged()), this, SLOT(modified())) ; // TODO check
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
