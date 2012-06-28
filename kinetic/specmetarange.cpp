#include "specmetarange.h"
#include "specmetavariable.h"
#include <qwt_plot.h>
#include <QMouseEvent>

specMetaRange::specMetaRange(double x1, double x2, specMetaVariable *par)
	: specRange(x1,x2),
	  parent(par),
	  selected(-1)
{
}

void specMetaRange::refreshPlotData()
{
	qDebug() << "refreshing meta plot data" ;
	specRange::refreshPlotData() ;
	if (parent)
		parent->rangeChanged(this);
}
