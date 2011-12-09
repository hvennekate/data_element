#include "specmetaview.h"

specMetaView::specMetaView(QWidget *parent) :
	specView(parent),
	dataView(0)
{
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
