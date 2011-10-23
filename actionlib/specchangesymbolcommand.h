#ifndef SPECCHANGESYMBOLCOMMAND_H
#define SPECCHANGESYMBOLCOMMAND_H

#include "specstylecommand.h"

class specChangeSymbolCommand : public specStyleCommand
{
public:
	specChangeSymbolCommand(specUndoCommand *parent = 0);
	void setStyle(specCanvasItem*) ;

	QDataStream& write(QDataStream &out) const ;
	QDataStream& read(QDataStream &in) ;
protected:
	void applyStyle(const specGenealogy&, int symbolNumber) ; // if int == -1: revert to old style
	int styleNo(specCanvasItem*) ;
	void saveStyles(const QList<specGenealogy>&) ;
private:
	qint32 getSymbol(specCanvasItem*) ;
	qint32 newSymbol ; // TODO do this with templates
	QVector<qint32> oldSymbols ;
};

#endif // SPECCHANGESYMBOLCOMMAND_H
