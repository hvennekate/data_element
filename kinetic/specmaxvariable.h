#ifndef SPECMAXVARIABLE_H
#define SPECMAXVARIABLE_H
#include "specmetavariable.h"

class specMaxVariable : public specMetaVariable
{
protected:
	double processPoints(QVector<QPointF> &points) const ;
};

#endif // SPECMAXVARIABLE_H
