#include "speckrcontextmenu.h"

specKRContextMenu::specKRContextMenu ( specCanvasItem *item)
		: QMenu ((QWidget*) (item->plot())), pointer(item)
{
	qDebug("initializing menu") ;
	deleteAction = new QAction(QIcon(":/remove.png"), tr("&Delete"), this);
	lineWidthAction = new QAction(QIcon(":/lineWidth.png"), tr("&Line width"), this);
	
	connect(deleteAction,SIGNAL(triggered()),this,SLOT(deleteItem())) ;
	connect(lineWidthAction,SIGNAL(triggered()),this,SLOT(changeLineWidth())) ;
	
	addAction(lineWidthAction) ;
	addAction(deleteAction) ;
}


specKRContextMenu::~specKRContextMenu()
{
}

void specKRContextMenu::deleteItem()
{
	qDebug("deleting pointer") ;
	if (pointer) delete pointer ;
	qDebug("pointer deleted") ;
}

void specKRContextMenu::changeLineWidth()
{
	if(pointer) pointer->setLineWidth() ;
}
