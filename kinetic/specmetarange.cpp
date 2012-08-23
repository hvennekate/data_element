#include "specmetarange.h"
#include "specmetavariable.h"
#include "specplot.h"
#include <QMouseEvent>
#include "canvaspicker.h"

specMetaRange::specMetaRange(double x1, double x2, specMetaVariable *par)
	: specRange(x1,x2),
	  parent(par)
{
}

specMetaRange::addressObject specMetaRange::address()
{
	return parent->address(this) ;
}

void specMetaRange::attach(QwtPlot *plot)
{
	specPlot *sp = qobject_cast<specPlot*>(plot) ;
	if (sp && sp->metaPicker())
		sp->metaPicker()->addSelectable(this) ;
	specRange::attach(plot) ;
}
