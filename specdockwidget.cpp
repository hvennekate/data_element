#include "specdockwidget.h"
#include <QEvent>
#include <QFileInfo>
#include <QMainWindow>
#include <QToolBar>
#include "specactionlibrary.h"
#include "specsplitter.h"
#include "specview.h"

#define APPLYTOSUBDOCKS foreach(specDockWidget* sub, subDocks)

specDockWidget::specDockWidget(QString type, QWidget *parent) :
	QDockWidget(parent),
	widgetTypeName(type),
	changingTitle(false)
{}

void specDockWidget::setupWindow(specActionLibrary *actions)
{
	// prepare main widget
	QMainWindow* widget = new QMainWindow(this) ;
	widget->setWindowFlags(Qt::Widget);
	setWidget(widget) ;

	// add inner widgets
	QList<QWidget*> innerWidgets = mainWidgets() ;
	if (innerWidgets.size() == 1)
		widget->setCentralWidget(innerWidgets.first()) ;
	else
	{
		specSplitter *splitter = new specSplitter(Qt::Vertical, this) ;
		foreach(QWidget* innerWidget, innerWidgets)
			splitter->insertWidget(0,innerWidget);
		widget->setCentralWidget(splitter);
	}

	// add toolbars
	if (!actions)
		return ;
	QList<QWidget*> allWidgets ;
	allWidgets << this << innerWidgets ;
	foreach(QWidget* w, allWidgets)
	{
		specView* view = dynamic_cast<specView*>(w);
		specPlot* plot = dynamic_cast<specPlot*>(w) ;
		if (view && view->model())
		{
			view->setActionLibrary(actions) ;
			actions->addDragDropPartner(view->model());
		}
		if (plot)
			actions->addPlot(plot) ;
		QToolBar* toolbar = actions->toolBar(w) ;
		if (toolbar->actions().isEmpty())
			delete toolbar ;
		else
			widget->addToolBar(toolbar);
	}

}

void specDockWidget::changeEvent(QEvent *event)
{
	if (changingTitle) return ;
	if (event->type() == QEvent::WindowTitleChange)
	{
		changingTitle = true ;
		setWindowTitle(QFileInfo(windowFilePath()).fileName()
			       + QLatin1String("[*]")
			       + QLatin1Char(' ') + QChar(0x2014) + QLatin1Char(' ')
			       + widgetTypeName) ;
		changingTitle = false ;
		event->accept();
	}
	QDockWidget::changeEvent(event) ;
}
