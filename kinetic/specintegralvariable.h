#ifndef SPECINTEGRALVARIABLE_H
#define SPECINTEGRALVARIABLE_H
#include "specmetavariable.h"

class specIntegralVariable : public specMetaVariable
{
protected:
	double processPoints(QVector<QPointF>& points) const ;
};

#endif // SPECINTEGRALVARIABLE_H
