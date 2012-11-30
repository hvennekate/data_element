#include "specmetarange.h"
#include "specmetavariable.h"
#include "specplot.h"
#include <QMouseEvent>
#include "canvaspicker.h"

specMetaRange::specMetaRange(double x1, double x2, double yinit, specMetaVariable *par)
    : specRange(x1,x2, yinit),
	  parent(par)
{
}

specMetaRange::addressObject specMetaRange::address()
{
	return parent->address(this) ;
}

void specMetaRange::attach(QwtPlot *newPlot)
{
	if (newPlot == plot()) return ;
	specPlot *sp = qobject_cast<specPlot*>(newPlot) ; // TODO this seems to cause crashes
	specPlot *oldPlot = qobject_cast<specPlot*>(plot()) ;
	if (oldPlot && oldPlot->metaPicker())
		oldPlot->metaPicker()->removeSelectable(this) ;
	if (sp && sp->metaPicker())
		sp->metaPicker()->addSelectable(this) ;
	specRange::attach(sp) ;
}

void specMetaRange::detach()
{
	specPlot *oldPlot = qobject_cast<specPlot*>(plot()) ;
	if (oldPlot && oldPlot->metaPicker())
		oldPlot->metaPicker()->removeSelectable(this) ;
	specRange::detach() ;
}

specMetaRange::~specMetaRange()
{
	detach() ;
}
