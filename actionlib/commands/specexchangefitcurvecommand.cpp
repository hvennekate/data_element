#include "specexchangefitcurvecommand.h"
#include "specfitcurve.h"

specExchangeFitCurveCommand::specExchangeFitCurveCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  curve(0)
{
}

void specExchangeFitCurveCommand::setup(const QModelIndex &i, specFitCurve *c)
{
	if (!parentObject()) return ;
	specMetaItem *pointer = dynamic_cast<specMetaItem*>(((specModel*) parentObject())->itemPointer(i)) ;
	if (!pointer) return ;
	if (curve) delete curve ;
	curve = c ;
	item = specGenealogy(QModelIndexList() << i) ;
}

void specExchangeFitCurveCommand::parentAssigned()
{
	if (!parentObject()) return ;
	item.setModel(qobject_cast<specModel*>(parentObject())) ;
}

void specExchangeFitCurveCommand::undoIt()
{
	doIt() ;
}

void specExchangeFitCurveCommand::doIt()
{
	specMetaItem *pointer = dynamic_cast<specMetaItem*>(item.firstItem()) ;
	if (!pointer) return ;
	item.model()->signalBeginReset(); // to get the selection right // TODO improve!
	curve = pointer->setFitCurve(curve);
	item.model()->signalChanged(item.firstIndex());
	item.model()->signalEndReset(); // to get the selection right
}

void specExchangeFitCurveCommand::writeCommand(QDataStream &out) const
{
	out << quint8((bool) curve) << item ;
	if (curve) out << *curve ;
}

void specExchangeFitCurveCommand::readCommand(QDataStream &in)
{
	delete curve ;
	quint8 hasCurve(0) ;
	in >> hasCurve >> item ;
	if(hasCurve)
	{
		curve = new specFitCurve ;
		in >> *curve ;
	}
	else
		curve = 0 ;
}

specExchangeFitCurveCommand::~specExchangeFitCurveCommand()
{
	delete curve ;
}
