#include "specdescriptorvariable.h"

QVector<double> specDescriptorVariable::values(specModelItem *, const QVector<double> &) const
{
	return QVector<double>(xvals.size(), item->descriptor(descriptor).toDouble()) ;
}
