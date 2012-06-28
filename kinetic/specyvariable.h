#ifndef SPECYVARIABLE_H
#define SPECYVARIABLE_H
#include "specmetavariable.h"

class specYVariable : public specMetaVariable
{
public:
    QVector<double> values(specModelItem *, const QVector<double> &) const ;
};

#endif // SPECYVARIABLE_H
