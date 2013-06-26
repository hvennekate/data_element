#ifndef SPECMAXPOSVARIABLE_H
#define SPECMAXPOSVARIABLE_H
#include "specmetavariable.h"

class specMaxPosVariable : public specMetaVariable
{
protected:
	double processPoints(QVector<QPointF> &points) const ;
};

#endif // SPECMAXPOSVARIABLE_H
