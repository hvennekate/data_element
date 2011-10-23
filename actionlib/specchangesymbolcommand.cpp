#include "specchangesymbolcommand.h"
#include <qwt_symbol.h>

specChangeSymbolCommand::specChangeSymbolCommand(specUndoCommand *parent)
	: specStyleCommand(parent)
{
}

void specChangeSymbolCommand::setStyle(specCanvasItem *item)
{
	qDebug("getting new symbol %d",getSymbol(item)) ;
	newSymbol = getSymbol(item) ;
}

qint32 specChangeSymbolCommand::getSymbol(specCanvasItem *item)
{
	int symbol = -2 ;
	if (const QwtSymbol *symPointer = item->symbol())
		symbol = symPointer->style() ;
	return symbol ;
}

int specChangeSymbolCommand::styleNo(specCanvasItem *item)
{
	int symbol = getSymbol(item) ;
	if(!oldSymbols.contains(symbol))
		oldSymbols << symbol ;
	return oldSymbols.indexOf(symbol) ;
}

void specChangeSymbolCommand::saveStyles(const QList<specGenealogy> &list)
{
	qDebug("saving old symbols") ;
	oldSymbols.clear() ;
	for (int i = 0 ; i < list.size() ; ++i)
		oldSymbols << getSymbol(list[i].items().first()) ;
}

void specChangeSymbolCommand::applyStyle(const specGenealogy &genealogy, int symbolNumber)
{
	qDebug("applying new symbol styles") ;
	int symbol = (symbolNumber < 0 || !(symbolNumber < oldSymbols.size()) ) ? newSymbol : oldSymbols[symbolNumber] ;
	QList<specModelItem*> items = genealogy.items() ;
	for (int j = 0 ; j < items.size() ; ++j)
	{
		specModelItem *item = items[j] ;
		item->setSymbol(symbol == -2 ? 0 : new QwtSymbol(QwtSymbol::Style(symbol),
				  item->brush(), item->pen(),QSize(5,5))); // TODO check for memory leak // TODO remove
	}
}

QDataStream& specChangeSymbolCommand::write(QDataStream &out) const
{
	out << qint32(newSymbol) << oldSymbols ;
	return (specStyleCommand::write(out) ) ;
}

QDataStream& specChangeSymbolCommand::read(QDataStream &in)
{
	in >> newSymbol >> oldSymbols ;
	return (specStyleCommand::read(in)) ;
}

