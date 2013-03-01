#include "specminposvariable.h"

double specMinPosVariable::processPoints(QVector<QPointF> &points) const
{
    double minX = NAN, minY = INFINITY ;
    foreach (const QPointF& point, points)
    {
        if (point.y() < minY)
        {
            minX = point.x() ;
            minY = point.y() ;
        }
    }
    return minX ;
}
