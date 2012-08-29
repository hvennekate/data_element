#include "specsplitter.h"
#include <QContextMenuEvent>

specSplitter::specSplitter(Qt::Orientation o, QWidget *parent)
	: QSplitter(o,parent)
{
}

specSplitter::specSplitter(QWidget *parent)
	: QSplitter(parent)
{
}

void specSplitter::contextMenuEvent(QContextMenuEvent *e)
{
	bool flip = false ;
	QWidget * child = childAt(e->pos()) ;
	for (int i = 0 ; i < count() ; ++i)
		flip = flip || (child == handle(i)) ;
	e->accept();
	if (!flip) return ;
	if (orientation() == Qt::Horizontal)
		setOrientation(Qt::Vertical) ;
	else
		setOrientation(Qt::Horizontal) ;
}
