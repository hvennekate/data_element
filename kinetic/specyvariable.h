#ifndef SPECYVARIABLE_H
#define SPECYVARIABLE_H
#include "specmetavariable.h"

class specYVariable : public specMetaVariable
{
public:
	bool xValues(specModelItem *, QVector<double> &) const ;
	QVector<double> values(specModelItem *, const QVector<double> &) const ;
};

#endif // SPECYVARIABLE_H
