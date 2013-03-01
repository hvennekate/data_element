#include "specmaxposvariable.h"

double specMaxPosVariable::processPoints(QVector<QPointF> &points) const
{
    double maxX = NAN, maxY = -INFINITY ;
    foreach (const QPointF& point, points)
    {
        if (point.y() > maxY)
        {
            maxX = point.x() ;
            maxY = point.y() ;
        }
    }
    return maxX ;
}
