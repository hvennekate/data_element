#ifndef SPECMINVARIABLE_H
#define SPECMINVARIABLE_H
#include "specmetavariable.h"

class specMinVariable : public specMetaVariable
{
protected:
    double processPoints(QVector<QPointF> &points) const
};

#endif // SPECMINVARIABLE_H
