#include "specexchangedatacommand.h"
#include "qwt_plot.h"

specExchangeDataCommand::specExchangeDataCommand(specUndoCommand* parent)
    : specSingleItemCommand
#ifdef WIN32BUILD
	      <specDataItem>
#endif
      (parent)
{
}

void specExchangeDataCommand::setItem(specModelItem* p, const QVector<specDataPoint>& newData)
{
	data = newData ;
	specSingleItemCommand
#ifdef WIN32BUILD
		<specDataItem>
#endif
		::setItem(p) ;
}

void specExchangeDataCommand::undoIt()
{
	doIt() ;
}

void specExchangeDataCommand::doIt()
{
	specDataItem* pointer = itemPointer() ;
	if(!pointer) return ;
	pointer->swapData(data);
}

void specExchangeDataCommand::writeCommand(QDataStream& out) const
{
	out << data ;
	writeItem(out) ;
}

void specExchangeDataCommand::readCommand(QDataStream& in)
{
	in >> data ;
	readItem(in) ;
}
