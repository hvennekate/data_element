#include "specminmaxvalidator.h"

specMinMaxValidator::specMinMaxValidator(QObject *parent)
 : QDoubleValidator(parent)
{
}


specMinMaxValidator::~specMinMaxValidator()
{
}

void specMinMaxValidator::setMax(const QString& max)
{ QDoubleValidator::setTop(max.toDouble()) ; }

void specMinMaxValidator::setMin(const QString& min)
{ QDoubleValidator::setBottom(min.toDouble()) ; }
