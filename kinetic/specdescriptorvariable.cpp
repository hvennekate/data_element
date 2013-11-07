#include "specdescriptorvariable.h"

QVector<double> specDescriptorVariable::values(specModelItem* item, const QVector<double>& xvals) const
{
	return QVector<double> (xvals.size(), item->descriptor(descriptor).toDouble()) ;
}
