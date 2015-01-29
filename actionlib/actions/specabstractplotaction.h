#ifndef SPECABSTRACTPLOTACTION_H
#define SPECABSTRACTPLOTACTION_H

#include "specundoaction.h"

class specPlot ;

class specAbstractPlotAction : public specUndoAction
{
	Q_OBJECT
private slots:
	void gotTrigger() ;
protected:
	specPlot *plot() const ;
public:
	explicit specAbstractPlotAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
};

#endif // SPECABSTRACTPLOTACTION_H
