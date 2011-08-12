#include "speckineticview.h"
#include "speckinetic.h"

specKineticView::specKineticView(QWidget* parent)
		: specView(parent)
{
	newItemAction = new QAction(QIcon(":/add.png"), tr("&New kinetic curve"), this);
	newItemAction->setShortcut(tr("Ctrl+I"));
	newItemAction->setStatusTip(tr("Create new kinetic curve"));
	connect(newItemAction, SIGNAL(triggered()), this, SLOT(newItem()));
	
	newRangeAction = new QAction(QIcon(":/newrange.png"), tr("&Add range to current kinetic curve"), this) ;
	newRangeAction->setStatusTip(tr("New range")) ;
	connect(newRangeAction,SIGNAL(triggered()), this, SLOT(newRange())) ;
}

QList<QAction*> specKineticView::actions()
{ return specView::actions() << newItemAction << newRangeAction ; }

specKineticView::~specKineticView()
{
}

void specKineticView::newItem() // TODO position etc.
{
	specKinetic *pointer = new specKinetic ;
	pointer->attach(plot) ;
	if(!model()->insertItems(QList<specModelItem*>() << pointer,currentIndex(),0))
	{
		pointer->detach() ;
		delete pointer ;
	}
}

void specKineticView::newRange()
{
	QModelIndex current = currentIndex() ; // TODO create separate function for validity test of pointer
	qDebug("checking if current is valid") ;
	if (!current.isValid()) return ;
	qDebug("checking if current is folder") ;
	if (((specModelItem*) current.internalPointer())->isFolder()) return ;
	qDebug("adding items to current") ;
	((specKinetic*) current.internalPointer())->newRange() ;
}

void specKineticView::addToCurrent(const QModelIndexList& list)
{
	QModelIndex current = currentIndex() ;
	qDebug("checking if current is valid") ;
	if (!current.isValid()) return ;
	qDebug("checking if current is folder") ;
	if (((specModelItem*) current.internalPointer())->isFolder()) return ;
	qDebug("adding items to current") ;
	((specKinetic*) current.internalPointer())->addItems(list) ;
}

specKineticModel* specKineticView::model () const
{ return (specKineticModel*) specView::model() ; }

void specKineticView::setModel(specKineticModel* mod)
{ specView::setModel(mod) ; }
