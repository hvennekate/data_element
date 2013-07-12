#include "specdockwidget.h"
#include <QEvent>
#include <QFileInfo>
#include <QMainWindow>
#include <QToolBar>
#include "specactionlibrary.h"
#include "specsplitter.h"
#include "specview.h"
#include "specitemaction.h"

#define APPLYTOSUBDOCKS foreach(specDockWidget* sub, subDocks)

specDockWidget::specDockWidget(QString type, QWidget *parent, bool floating) :
	QDockWidget(parent),
	widgetTypeName(type),
	Plot(0),
	changingTitle(false)
{
	setFloating(floating);
}

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
			connect(view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
					this, SLOT(selectionChanged(QItemSelection,QItemSelection))) ;
		}
		if (plot)
		{
			Plot = plot ;
			actions->addPlot(plot) ;
		}
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

void specDockWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	foreach(QModelIndex index, deselected.indexes())
	{
		if ( !index.isValid() ) continue ;
		if ( index.column() ) continue ;
		specModelItem* pointer = (specModelItem*) index.internalPointer() ;
		pointer->detach();
		-- selectedTypes[pointer->typeId()] ;
	}

	foreach(QModelIndex index, selected.indexes())
	{
		if ( !index.isValid() ) continue ;
		if ( index.column() ) continue ;
		specModelItem* pointer = (specModelItem*) index.internalPointer() ;
		pointer->attach(Plot);
		++ selectedTypes[pointer->typeId()] ;
	}

	if (Plot) Plot->replot();

	foreach(specItemAction* action, findChildren<specItemAction*>())
	{
		QList<specStreamable::type> ts = action->requiredTypes() ;
		bool enable = ts.isEmpty();
		foreach(specStreamable::type t, ts)
			enable = enable || selectedTypes[t] ;
		action->setEnabled(enable && action->requirements()) ;
	}
}
