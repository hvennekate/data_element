#ifndef SPECPRINTPLOTACTION_H
#define SPECPRINTPLOTACTION_H

#include "specundoaction.h"

class QPrinter ;

class specPrintPlotAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specPrintPlotAction(QObject* parent = 0);
	~specPrintPlotAction() ;
	const std::type_info& possibleParent() ;
private:
	void execute() ;
	QPrinter* printer ;
};

#endif // SPECPRINTPLOTACTION_H
