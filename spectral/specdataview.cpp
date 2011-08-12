#include "specdataview.h"
#include <QTextStream>

specDataView::specDataView(QWidget* parent)
		: specView(parent)
{
	
	importAction = new QAction(QIcon(":/fileimport.png"), tr("&Import"), this);
	importAction->setShortcut(tr("Ctrl+I"));
	importAction->setStatusTip(tr("Import a file"));
	connect(importAction, SIGNAL(triggered()), this, SLOT(importFile()));
	
/*	getPersistent = new QAction(QIcon(), tr("&getPersistent"), this) ;
	getPersistent->setStatusTip("getPersistent") ;
	connect(getPersistent, SIGNAL(triggered()), this, SLOT(aqPersistent())) ;
	
	printPersistent = new QAction(QIcon(), tr("&printPersistent"), this) ;
	printPersistent->setStatusTip("printPersistent") ;
	connect(printPersistent, SIGNAL(triggered()), this, SLOT(outputPersistent())) ;
	*/
}

/*void specDataView::aqPersistent()
{
	index = currentIndex() ;
}*/

/*void specDataView::outputPersistent()
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
	QModelIndex parent = index ;
// 	cout << "Persistent hierarchy:  " ;
	while (parent.isValid())
	{
// 		cout << parent.row() << "  " ;
		parent = parent.parent() ;
	}
// 	cout << endl ;
}*/

void specDataView::importFile()
{ model()->importFile(selectionModel()->currentIndex()) ; }

QList<QAction*> specDataView::actions()
{ return specView::actions() << importAction ; }

specDataView::~specDataView()
{
}


