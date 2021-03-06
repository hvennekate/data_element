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

specDockWidget::specDockWidget(QString type, QWidget* parent, bool floating) :
	QDockWidget(parent),
	widgetTypeName(type),
	Plot(0),
	changingTitle(false)
{
	setFloating(floating);
	setVisible(false) ;
}

void specDockWidget::setupWindow(specActionLibrary* actions)
{
	// prepare main widget
	QMainWindow* widget = new QMainWindow(this) ;
	widget->setWindowFlags(Qt::Widget);
	setWidget(widget) ;

//	// get brushes
//	QPalette pal = palette() ;
//	QBrush activeWindow = pal.brush(QPalette::Active, QPalette::Window),
//			inactiveWindow = pal.brush(QPalette::Inactive, QPalette::Window) ;
//	if (activeWindow == inactiveWindow)
//		inactiveWindow.setColor(inactiveWindow.color().darker()) ;
//
//	pal.setBrush(QPalette::Inactive, QPalette::Window, inactiveWindow) ;
//	setPalette(pal) ;

	// add inner widgets
	QList<QWidget*> innerWidgets = mainWidgets() ;
	if(innerWidgets.size() == 1)
		widget->setCentralWidget(innerWidgets.first()) ;
	else
	{
		specSplitter* splitter = new specSplitter(Qt::Vertical, this) ;
		foreach(QWidget * innerWidget, innerWidgets)
		splitter->insertWidget(0, innerWidget);
		widget->setCentralWidget(splitter);
	}

	// add toolbars
	if(!actions)
		return ;
	QList<QWidget*> allWidgets ;
	allWidgets << this << innerWidgets ;
	foreach(QWidget * w, allWidgets)
	{
		specView* view = dynamic_cast<specView*>(w);
		specPlot* plot = dynamic_cast<specPlot*>(w) ;
		if(view && view->model())
		{
			view->setActionLibrary(actions) ;
			actions->addDragDropPartner(view->model());
			connect(view->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
				this, SLOT(selectionChanged(QItemSelection, QItemSelection))) ;
//			connect(view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//				this, SLOT(currentChanged(QModelIndex,QModelIndex))) ;
		}
		if(plot)
		{
			Plot = plot ;
			actions->addPlot(plot) ;
		}
		QToolBar* toolbar = actions->toolBar(w) ;
		if(toolbar->actions().isEmpty())
			delete toolbar ;
		else
			widget->addToolBar(toolbar);

//		w->setPalette(pal);
	}

	specView* view = findChild<specView*>() ;
	if(!view) return ;
	selectionChanged(view->selectionModel()->selection(), QItemSelection());
}

void specDockWidget::changeEvent(QEvent* event)
{
	if(changingTitle) return ;
	if(event->type() == QEvent::WindowTitleChange)
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
	toggleViewAction()->setText(tr("Show \"") + widgetTypeName + tr("\" window"));
}

void specDockWidget::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (Plot) Plot->haltPlotting(true);

	foreach(QModelIndex index, deselected.indexes())
	{
		if(!index.isValid()) continue ;
		if(index.column()) continue ;
		specModelItem* pointer = (specModelItem*) index.internalPointer() ;
		pointer->detach();
		-- selectedTypes[pointer->typeId()] ;
	}

	foreach(QModelIndex index, selected.indexes())
	{
		if(!index.isValid()) continue ;
		if(index.column()) continue ;
		specModelItem* pointer = (specModelItem*) index.internalPointer() ;
		pointer->attach(Plot);
		++ selectedTypes[pointer->typeId()] ;
	}

	if(Plot) Plot->haltPlotting(false) ;

	foreach(specItemAction * action, findChildren<specItemAction*>())
	{
		QList<specStreamable::type> ts = action->requiredTypes() ;
		bool enable = ts.isEmpty();
		foreach(specStreamable::type t, ts)
			enable = enable || selectedTypes[t] ;
		action->setEnabled(enable && action->requirements()) ;
		action->setChecked(action->checkRequirements());
	}
}

//void specDockWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous) // TODO why does this not work?
//{
//	foreach (specItemAction* action, findChildren<specItemAction*>())
//	{
//		action->setEnabled(action->isEnabled() && action->requirements());
//		action->setCheckable(action->checkRequirements());
//	}
//}
