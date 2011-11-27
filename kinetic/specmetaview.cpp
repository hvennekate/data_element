#include "specmetaview.h"

specMetaView::specMetaView(QWidget *parent) :
    specView(parent)
{
}

specMetaView::~specMetaView()
{
}

void specMetaView::setModel(specMetaModel *mod)
{
	specView::setModel(mod) ;
}

specMetaModel *specMetaView::model() const
{
	return (specMetaModel*) specView::model() ;
}
