#include "specxvariable.h"

QVector<double> specXVariable::values(specModelItem* item, const QVector<double>& xvals) const
{
	Q_UNUSED(item) ;
	return xvals ;
}

// TODO common parent xyvariable

bool specXVariable::xValues(specModelItem* item, QVector<double>& xvals) const  // false if vector was empty
{
	return extractXs(item, xvals) ;
}
