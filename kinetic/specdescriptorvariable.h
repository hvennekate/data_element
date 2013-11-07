#ifndef SPECDESCRIPTORVARIABLE_H
#define SPECDESCRIPTORVARIABLE_H
#include "specmetavariable.h"

class specDescriptorVariable : public specMetaVariable
{
public:
	QVector<double> values(specModelItem*, const QVector<double>&) const ;
};

#endif // SPECDESCRIPTORVARIABLE_H
