#include "speckineticwidget.h"
#include <QTextStream>
#include "speccanvasitem.h"
#include "specgenericmimeconverter.h"

specMetaView* specKineticWidget::view()
{ return items ; }

void specKineticWidget::contextMenuEvent(QContextMenuEvent* event)
{
	int a,b,c,d ;
	splitter->geometry().getCoords(&a,&b,&c,&d) ;
	if(splitter->geometry().contains(event->x(),event->y()))
	{
		if (splitter->orientation() == Qt::Horizontal)
			splitter->setOrientation(Qt::Vertical) ;
		else
			splitter->setOrientation(Qt::Horizontal) ;
	}
}

specKineticWidget::specKineticWidget(QString title, QWidget *parent)
	: QDockWidget(title, parent)
{
	setFloating(true) ;
	content = new QWidget ;
	layout = new QVBoxLayout ;
	plot = new specPlot ;
	splitter = new QSplitter ;
	items = new specMetaView(this) ;
	
	plot->setMinimumHeight(100) ;
	plot->setAxisTitle(QwtPlot::yLeft,"cm<sup>-1</sup> <font face=\"Symbol\">D</font>mOD") ;
	plot->setAxisTitle(QwtPlot::xBottom,"ps") ;
	
	items->setModel(new specMetaModel(items)) ;
	new specGenericMimeConverter(items->model());
	
	splitter->setOrientation(Qt::Vertical) ;
	splitter->addWidget(plot) ;
	splitter->addWidget(items) ;
	
	layout->addWidget(splitter) ;
	layout->setContentsMargins(0,0,0,0) ;
	
	content->setLayout(layout) ;
	setWidget(content) ;

	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;
}

void specKineticWidget::writeToStream(QDataStream &out) const
{
	out << *plot << *items ;
}

void specKineticWidget::readFromStream(QDataStream &in)
{
	in >> *plot >> *items ;
}

specKineticWidget::~specKineticWidget()
{
}

void specKineticWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	foreach(QModelIndex index, deselected.indexes())
		if (!index.column() && index.isValid() && !((specModelItem*) index.internalPointer())->isFolder())
			((specCanvasItem*) index.internalPointer())->detach() ; // TODO create kinetic folder or extend specFolderItem
	
	foreach(QModelIndex index, selected.indexes())
		if (!index.column() && !((specModelItem*) index.internalPointer())->isFolder())
			((specCanvasItem*) index.internalPointer())->attach(plot) ;
	plot->replot() ;
}

// TODO crash when another meta item is added after the first has been assigned servers

void specKineticWidget::addToolbar(specActionLibrary* actions)
{
	actions->addDragDropPartner(items->model());
	actions->addPlot(plot) ;
	QToolBar *toolbar = actions->toolBar(items) ;
	toolbar->addSeparator() ;
	QToolBar *plotBar = actions->toolBar(plot) ;
	plotBar->addActions(plot->actions()) ;
	layout->insertWidget(0,toolbar) ;
	layout->insertWidget(1,plotBar);
}
