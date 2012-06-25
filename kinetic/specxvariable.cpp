#include "specxvariable.h"

QVector<double> specXVariable::values(specModelItem *item, const QVector<double> &xvals) const
{
	Q_UNUSED(item) ;
	return xvals ;
}
