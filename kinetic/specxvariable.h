#ifndef SPECXVARIABLE_H
#define SPECXVARIABLE_H
#include "specmetavariable.h"

class specXVariable : public specMetaVariable
{
public:
	QVector<double> values(specModelItem *, const QVector<double> &) const ;
};

#endif // SPECXVARIABLE_H
