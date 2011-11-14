#include "speckineticwidget.h"
#include "speckinetic.h"
#include <QTextStream>

specKineticView* specKineticWidget::view()
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
	: QDockWidget(title, parent ? (QWidget*) parent->parent() : NULL), currentKinetic(0), directParent(parent)
{
	setFloating(true) ;
	content = new QWidget ;
	layout = new QVBoxLayout ;
	plot = new specPlot ;
	splitter = new QSplitter ;
	items = new specKineticView(this) ;
	toolbar = new QToolBar ;
	
// 	QTextStream cout(stdout, QIODevice::WriteOnly) ;
// 	cout << "current parent:  " << items->parent() << endl ;
	toolbar-> setContentsMargins(0,0,0,0) ;
	toolbar-> setIconSize(QSize(16,16)) ;
	
	plot->setMinimumHeight(100) ;
	plot->setAxisTitle(QwtPlot::yLeft,"cm<sup>-1</sup> <font face=\"Symbol\">D</font>mOD") ;
	plot->setAxisTitle(QwtPlot::xBottom,"ps") ;
	
	items->setModel(new specKineticModel(items)) ;
	items->model()->setMimeTypes(QStringList("application/spec.kinetic.item")) ;
	qDebug() << "Kinetic model" << items->model() ;
	
	splitter->setOrientation(Qt::Vertical) ;
	splitter->addWidget(plot) ;
	splitter->addWidget(items) ;
	
	setupActions() ;
	
	layout->addWidget(toolbar) ;
	layout->addWidget(splitter) ;
	layout->setContentsMargins(0,0,0,0) ;
	
	content->setLayout(layout) ;
	setWidget(content) ;
	
	items->plot = plot ; // TODO purge
	
	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;
	connect(items->selectionModel(),SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&))) ;
	
// 	cout << "New parent:  "  << items->parent()->parent()->parent() << "  " << content << endl ;
}

void specKineticWidget::setupActions()
{
// 	syncAction = new QAction(QIcon(":/synckinetic"), tr("Synchronize with main window"), this) ;
// 	connect(syncAction,SIGNAL(triggered()),this,SLOT(syncCurrent())) ;
	
	foreach(QAction *pointer, items->actions())
		toolbar->addAction(pointer) ;
	
// 	toolbar->addAction(syncAction) ;
}

QDataStream& operator<<(QDataStream& stream, const specKineticWidget& widget)
{
	stream << *(widget.items->model()) ;
}

QDataStream& operator>>(QDataStream& stream, specKineticWidget& widget)
{
	stream >> *(widget.items->model()) ;
}

specKineticWidget::~specKineticWidget()
{
}

void specKineticWidget::addKinetic()
{
	items->model()->insertItems(QList<specModelItem*>() << new specKinetic(),items->currentIndex(),0) ; // TODO move this to view
	// TODO maybe make view dependent on type of view/subclass view
}

void specKineticWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	foreach(QModelIndex index, deselected.indexes())
		if (!index.column() && index.isValid() && !((specModelItem*) index.internalPointer())->isFolder())
			((specKinetic*) index.internalPointer())->hideRanges() ; // TODO create kinetic folder or extend specFolderItem
	
	foreach(QModelIndex index, selected.indexes())
		if (!index.column() && !((specModelItem*) index.internalPointer())->isFolder())
			((specKinetic*) index.internalPointer())->showRanges(plot) ; 
	
	plot->replot() ;
}

/*void specKineticWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	foreach(QModelIndex index, deselected.indexes())
		if (!index.column() && index.isValid())
			((specModelItem*) index.internalPointer())->detach() ;
	
	foreach(QModelIndex index, selected.indexes())
		if (!index.column())
			((specModelItem*) index.internalPointer())->attach(plot) ;
	
	plot->replot() ;
}*/
