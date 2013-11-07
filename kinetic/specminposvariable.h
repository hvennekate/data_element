#ifndef SPECMINPOSVARIABLE_H
#define SPECMINPOSVARIABLE_H
#include "specmetavariable.h"

class specMinPosVariable : public specMetaVariable
{
protected:
	double processPoints(QVector<QPointF>& points) const ;
};

#endif // SPECMINPOSVARIABLE_H
