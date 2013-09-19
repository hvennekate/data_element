#include "specplotmovecommand.h"
#include "specexchangefiltercommand.h"
#include "specmulticommand.h"

specPlotMoveCommand::specPlotMoveCommand(specUndoCommand* parent)
	: specSingleItemCommand<specModelItem>(parent),
	  alternativeCommand(0)
{
}

specStreamable::type specPlotMoveCommand::typeId() const
{
	if (alternativeCommand) return alternativeCommand->typeId() ;
	return specStreamable::movePlotCommandId ;
}

void specPlotMoveCommand::doIt()
{
	if (alternativeCommand)
	{
		alternativeCommand->redo();
		return ;
	}
	specModelItem* item = itemPointer() ;
	if (!item) return ;
	alternativeCommand = specExchangeFilterCommand::generateFilterExchanger(item) ;
	item->moveXBy(shift) ;
	item->scaleBy(scale) ; // TODO prevent scaling by zero.
	item->moveYBy(offset) ;
	item->addToSlope(slope) ;
}

void specPlotMoveCommand::undoIt()
{
	if (alternativeCommand)
	{
		alternativeCommand->undo();
		return ;
	}
	specModelItem* item = itemPointer() ;
	if (!item) return ;
	alternativeCommand = specExchangeFilterCommand::generateFilterExchanger(item);
	item->addToSlope(-slope) ;
	item->moveYBy(-offset) ;
	item->scaleBy(1./scale) ;
	item->moveXBy(-shift) ;
}

void specPlotMoveCommand::writeCommand(QDataStream &out) const
{
	if (alternativeCommand)
		alternativeCommand->writeCommand(out) ;
	else
	{
		out << slope << offset << scale << shift ;
		writeItem(out) ;
	}
}

void specPlotMoveCommand::readCommand(QDataStream &in)
{
	in >> slope >> offset >> scale >> shift ;
	readItem(in) ;
	generateDescription();
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
