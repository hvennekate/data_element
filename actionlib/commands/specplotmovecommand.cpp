#include "specplotmovecommand.h"

specPlotMoveCommand::specPlotMoveCommand(specUndoCommand* parent)
	: specSingleItemCommand<specModelItem>(parent)
{
}

void specPlotMoveCommand::doIt()
{
	specModelItem* item = itemPointer() ;
	if (!item) return ;
	item->moveXBy(shift) ;
	item->scaleBy(scale) ; // TODO prevent scaling by zero.
	item->moveYBy(offset) ;
	item->addToSlope(slope) ;
}

void specPlotMoveCommand::undoIt()
{
	specModelItem* item = itemPointer() ;
	if (!item) return ;
	item->addToSlope(-slope) ;
	item->moveYBy(-offset) ;
	item->scaleBy(1./scale) ;
	item->moveXBy(-shift) ;
}


bool specPlotMoveCommand::mergeWith(const QUndoCommand* ot)
{
	if (!parentObject()) return false ;
	const specPlotMoveCommand *other = (const specPlotMoveCommand*) ot ;
	if (!mergeable(other)) return false ;
	if (!isfinite(other->scale) || !isfinite(scale)) return false ;
	slope = slope  * other->scale + other->slope ;
	offset= offset * other->scale + other->offset ;
	scale *= other->scale ;
	shift += other->shift ;
	generateDescription();
	return true ;
}

void specPlotMoveCommand::setCorrections(double xShift, double yOffset, double ySlope, double yScale)
{
	slope = isfinite(ySlope) ? ySlope : 0. ;
	offset = isfinite(yOffset) ? yOffset : 0. ;
	shift = isfinite(xShift) ? xShift : 0. ;
	scale = isnormal(yScale) ? yScale : 1. ;
	generateDescription();
}

void specPlotMoveCommand::writeCommand(QDataStream &out) const
{
	out << slope << offset << scale << shift ;
	writeItem(out) ;
}

void specPlotMoveCommand::readCommand(QDataStream &in)
{
	in >> slope >> offset >> scale >> shift ;
	readItem(in) ;
	generateDescription();
}

bool specPlotMoveCommand::mergeable(const specUndoCommand *other)
{
	return (((specPlotMoveCommand*) other)->itemPointer()) == itemPointer() ;
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
