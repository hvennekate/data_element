#include "speckineticwidget.h"
#include <QTextStream>
#include "speccanvasitem.h"
#include "specgenericmimeconverter.h"
#include "specmimetextexporter.h"
#include "specsplitter.h"
#include "canvaspicker.h"

specMetaView* specKineticWidget::view()
{ return items ; }

specKineticWidget::specKineticWidget(QString title, QWidget *parent)
	: QDockWidget(title, parent)
{
	setWhatsThis(tr("Meta dock widget - In this widget, further processing of the primary data can be done (integration, max, min, etc.)"));
	setFloating(true) ;
	content = new QWidget ;
	layout = new QVBoxLayout ;
	plot = new specPlot ;
	splitter = new specSplitter(Qt::Vertical,this) ;
	items = new specMetaView(this) ;
	
	plot->setMinimumHeight(100) ;
//	plot->setAxisTitle(QwtPlot::yLeft,"cm<sup>-1</sup> <font face=\"Symbol\">D</font>mOD") ;
//	plot->setAxisTitle(QwtPlot::xBottom,"ps") ;
	
	items->setModel(new specMetaModel(items)) ;
	new specGenericMimeConverter(items->model());
	new specMimeTextExporter(items->model()) ;

	
	splitter->setOrientation(Qt::Vertical) ;
	splitter->addWidget(plot) ;
	splitter->addWidget(items) ;
	
	layout->addWidget(splitter) ;
	layout->setContentsMargins(0,0,0,0) ;
	
	content->setLayout(layout) ;
	setWidget(content) ;

	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;

    connect(plot->svgAction(),SIGNAL(toggled(bool)),this,SLOT(svgModification(bool))) ;
    svgModification(false) ;
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
	plot->setUndoPartner(actions);
	QToolBar *toolbar = actions->toolBar(items) ;
	toolbar->addSeparator() ;
	QToolBar *plotBar = actions->toolBar(plot) ;
	plotBar->addActions(plot->actions()) ;
	layout->insertWidget(0,toolbar) ;
	layout->insertWidget(1,plotBar);
}

// TODO shift to parent class of kineticWidget and plotWidget
void specKineticWidget::svgModification(bool mod)
{
    if (mod) connect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;
    else disconnect(plot->svgPicker(),SIGNAL(pointMoved(specCanvasItem*,int,double,double)),items->model(), SLOT(svgMoved(specCanvasItem*,int,double,double))) ;

    plot->svgPicker()->highlightSelectable(mod) ;
}
