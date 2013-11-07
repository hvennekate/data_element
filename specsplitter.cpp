#include "specsplitter.h"
#include <QContextMenuEvent>

specSplitter::specSplitter(Qt::Orientation o, QWidget* parent)
	: QSplitter(o, parent)
{
}

specSplitter::specSplitter(QWidget* parent)
	: QSplitter(parent)
{
}

specSplitterHandle::specSplitterHandle(Qt::Orientation orientation, QSplitter* parent)
	: QSplitterHandle(orientation, parent)
{}

QSplitterHandle* specSplitter::createHandle()
{
	return new specSplitterHandle(orientation(), this) ;
}

void specSplitterHandle::mousePressEvent(QMouseEvent* e)
{
	QSplitter* p = qobject_cast<QSplitter*> (parent()) ;
	if(p && e->button() == Qt::RightButton && e->modifiers() == Qt::NoModifier)
		p->setOrientation(p->orientation() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal) ;
	QSplitterHandle::mousePressEvent(e) ;
}
