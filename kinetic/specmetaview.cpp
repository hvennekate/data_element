#include "specmetaview.h"
#include "specmetadelegate.h"

specMetaView::specMetaView(QWidget *parent) :
	specView(parent),
	dataView(0)
{
	QAbstractItemDelegate *olddel = itemDelegate() ;
	setItemDelegate(new specMetaDelegate) ;
	delete olddel ;
}

specMetaView::~specMetaView()
{
}

void specMetaView::setModel(specMetaModel *mod)
{
	specView::setModel((specModel*) mod) ;
}

specMetaModel *specMetaView::model() const
{
	return (specMetaModel*) specView::model() ;
}

void specMetaView::assignDataView(specView *view)
{
	dataView = view ;
}

specView *specMetaView::getDataView()
{
	return dataView ;
}
