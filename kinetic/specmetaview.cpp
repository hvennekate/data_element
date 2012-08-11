#include "specmetaview.h"
#include "specmetadelegate.h"
#include "specmetarange.h"
#include "actionlib/specmetarangecommand.h"
#include "specmetamodel.h"
#include "specmetaitem.h"

specMetaView::specMetaView(QWidget *parent) :
	specView(parent),
	dataView(0),
	undoPartner(0)
{
	QAbstractItemDelegate *olddel = itemDelegate() ;
	setItemDelegate(new specMetaDelegate) ;
	delete olddel ;
}

void specMetaView::setUndoPartner(specActionLibrary *l)
{
	undoPartner = l ;
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

void specMetaView::rangeModified(specCanvasItem *r, int p, double x, double y)
{
	specMetaRange::addressObject address = ((specMetaRange*) r)->address() ;
	specMetaRangeCommand* command = new specMetaRangeCommand ;
	command->setItem(model()->index(address.item),address.range, address.variable, p, x, y) ;
	command->setParentObject(this) ;
	if (!undoPartner)
	{
		command->redo();
		delete command ;
	}
	else
		undoPartner->push(command) ;
}
