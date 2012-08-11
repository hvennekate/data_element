#include "specmetarange.h"
#include "specmetavariable.h"
#include <qwt_plot.h>
#include <QMouseEvent>

specMetaRange::specMetaRange(double x1, double x2, specMetaVariable *par)
	: specRange(x1,x2),
	  parent(par)
{
}

specMetaRange::addressObject specMetaRange::address()
{
	return parent->address(this) ;
}
