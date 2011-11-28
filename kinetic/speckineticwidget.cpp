#include "speckineticwidget.h"
#include <QTextStream>

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
	: QDockWidget(title, parent ? (QWidget*) parent->parent() : NULL), directParent(parent) // TODO looks fuzzy
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
	items->model()->setMimeTypes(QStringList("application/spec.kinetic.item")) ;
	qDebug() << "Kinetic model" << items->model() ;
	
	splitter->setOrientation(Qt::Vertical) ;
	splitter->addWidget(plot) ;
	splitter->addWidget(items) ;
	
	layout->addWidget(splitter) ;
	layout->setContentsMargins(0,0,0,0) ;
	
	content->setLayout(layout) ;
	setWidget(content) ;

	connect(items->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionChanged(const QItemSelection&, const QItemSelection&))) ;
	connect(items->selectionModel(),SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&))) ;
}

QDataStream& operator<<(QDataStream& stream, const specKineticWidget& widget)
{
//	stream << *(widget.items->model()) ;
	return stream ; // TODO
}

QDataStream& operator>>(QDataStream& stream, specKineticWidget& widget)
{
//	stream >> *(widget.items->model()) ;
	return stream ; // TODO
}

specKineticWidget::~specKineticWidget()
{
}

void specKineticWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
//	foreach(QModelIndex index, deselected.indexes())
//		if (!index.column() && index.isValid() && !((specModelItem*) index.internalPointer())->isFolder())
//			((specKinetic*) index.internalPointer())->hideRanges() ; // TODO create kinetic folder or extend specFolderItem
	
//	foreach(QModelIndex index, selected.indexes())
//		if (!index.column() && !((specModelItem*) index.internalPointer())->isFolder())
//			((specKinetic*) index.internalPointer())->showRanges(plot) ;
	
	plot->replot() ;
}

