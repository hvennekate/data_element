#include "specexchangefitcurvecommand.h"
#include "specfitcurve.h"

specExchangeFitCurveCommand::specExchangeFitCurveCommand(specUndoCommand *parent)
	: specSingleItemCommand(parent),
	  curve(0)
{
}

void specExchangeFitCurveCommand::setup(specModelItem *i, specFitCurve *c)
{
	if (curve) delete curve ;
	curve = c ;
	setItem(i) ;
}

void specExchangeFitCurveCommand::undoIt()
{
	doIt() ;
}

void specExchangeFitCurveCommand::doIt()
{
	specMetaItem *pointer = itemPointer() ;
	if (!pointer) return ;
	model()->signalBeginReset(); // to get the selection right // TODO improve!
	curve = pointer->setFitCurve(curve);
	if (curve) curve->detach();
	model()->signalChanged(model()->index(itemPointer()));
}

void specExchangeFitCurveCommand::writeCommand(QDataStream &out) const
{
	out << quint8((bool) curve) ;
	writeItem(out);
	if (curve) out << *curve ;
}

void specExchangeFitCurveCommand::readCommand(QDataStream &in)
{
	delete curve ;
	quint8 hasCurve(0) ;
	in >> hasCurve ;
	readItem(in) ;
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
