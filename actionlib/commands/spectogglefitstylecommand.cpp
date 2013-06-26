#include "spectogglefitstylecommand.h"

specToggleFitStyleCommand::specToggleFitStyleCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

void specToggleFitStyleCommand::setup(const QModelIndex & i)
{
	if (!parentObject()) return ;
	specMetaItem *pointer = dynamic_cast<specMetaItem*>(((specModel*) parentObject())->itemPointer(i)) ;
	if (!pointer) return ;
	item = specGenealogy(QModelIndexList() << i) ;
}

void specToggleFitStyleCommand::parentAssigned()
{
	//	if (!parentObject()) return ;
	item.setModel(qobject_cast<specModel*>(parentObject()));
}

void specToggleFitStyleCommand::undoIt()
{
	doIt() ;
}

void specToggleFitStyleCommand::doIt()
{
	specMetaItem *pointer = dynamic_cast<specMetaItem*>(item.firstItem()) ;
	if (!pointer) return ;
	pointer->toggleFitStyle() ;
}

void specToggleFitStyleCommand::writeCommand(QDataStream &out) const
{
	out << item ;
}

void specToggleFitStyleCommand::readCommand(QDataStream &in)
{
	in >> item ;
}
