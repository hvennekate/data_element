#include "specplotmovecommand.h"

specPlotMoveCommand::specPlotMoveCommand(specUndoCommand* parent)
	: specUndoCommand(parent),
	  items(0)
{
}

void specPlotMoveCommand::doIt()
{
	if (!items) return ;
	items->seekParent() ;
	if (!items->valid()) return ;
	foreach(specCanvasItem* item, items->items())
	{
		item->moveXBy(shift) ;
		item->scaleBy(scale) ; // TODO prevent scaling by zero.
		item->moveYBy(offset) ;
		item->addToSlope(slope) ;
	}
}

void specPlotMoveCommand::undoIt()
{
	if (!items) return ;
	items->seekParent() ;
	if (!items->valid()) return ;
	foreach(specCanvasItem* item, items->items())
	{
		item->addToSlope(-slope) ;
		item->moveYBy(-offset) ;
		item->scaleBy(1./scale) ;
		item->moveXBy(-shift) ;
	}
}


bool specPlotMoveCommand::mergeWith(const QUndoCommand* ot)
{
    if (!parentObject()) return false ;
	const specPlotMoveCommand *other = (const specPlotMoveCommand*) ot ;
	if (! (this->items && other->items)) return false ;
	if (*(other->items) != *(this->items)) return false ;
	slope += other->slope * scale ;
	offset+= other->offset * scale - other->slope * scale * shift ;
	scale *= other->scale ;
	shift += other->shift ;
	generateDescription();
	return true ;
}

void specPlotMoveCommand::setItem(QModelIndex index)
{
	if (items) delete items ;
	items = new specGenealogy(QModelIndexList() << index) ;
}

void specPlotMoveCommand::setCorrections(double xShift, double yOffset, double ySlope, double yScale)
{
	slope = ySlope ;
	offset = yOffset ;
	shift = xShift ;
	scale = yScale ;
	generateDescription();
}

void specPlotMoveCommand::writeCommand(QDataStream &out) const
{
	out << slope << offset << scale << shift << *items ;
}

void specPlotMoveCommand::readCommand(QDataStream &in)
{
	if (!items) items = new specGenealogy ;
	in >> slope >> offset >> scale >> shift >> *items ;
	generateDescription();
}

bool specPlotMoveCommand::mergeable(const specUndoCommand *other)
{
	return *(((specPlotMoveCommand*) other)->items) == *items ;
}

void specPlotMoveCommand::generateDescription()
{
	QStringList modificationDescriptions ;
	if (scale != 1.0) modificationDescriptions << QObject::tr("scaling by ") + QString::number(scale) ;
	if (offset) modificationDescriptions << QObject::tr("offset by ") + QString::number(offset) ;
	if (slope) modificationDescriptions << QObject::tr("slope by ") + QString::number(slope) ;
	if (shift) modificationDescriptions << QObject::tr("shifted x by ") + QString::number(shift) ;
	if (modificationDescriptions.size() > 1) modificationDescriptions.last().prepend(QObject::tr("and "));
	setText(QObject::tr("Modify data: ") +
			 modificationDescriptions.join(modificationDescriptions.size() > 2 ? ", " : " "));
}

QString specPlotMoveCommand::description() const
{
	return QString() ;
}

void specPlotMoveCommand::parentAssigned()
{
	if (items) items->setModel(qobject_cast<specModel*>(parentObject()));
}
