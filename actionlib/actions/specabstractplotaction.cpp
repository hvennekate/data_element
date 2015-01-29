#include "specabstractplotaction.h"
#include "specplot.h"

void specAbstractPlotAction::gotTrigger()
{
	if (!plot()) return ;
	execute();
}

specPlot *specAbstractPlotAction::plot() const
{
	return qobject_cast<specPlot*> (parentWidget()) ;
}

specAbstractPlotAction::specAbstractPlotAction(QObject *parent)
	: specUndoAction(parent)
{
}

const std::type_info &specAbstractPlotAction::possibleParent()
{
	return typeid(specPlot) ;
}
