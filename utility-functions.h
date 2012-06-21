#ifndef SPECUTILITIES_H
#define SPECUTILITIES_H

#include <QList>
#include <QVector>
#include <QHash>
#include <QString>
#include <QStringList>
#include "specdataitem.h"
#include <QTextStream>
#include <QFile>

// bool roundedEqual(double a, double b) ;
QList<specModelItem*> readLogFile(QFile& file) ;
QList<specModelItem*> readHVFile(QFile&) ;
QList<specModelItem*> readPEFile(QFile&) ;
QList<specModelItem*> readJCAMPFile(QFile&) ;
QPair<QString,specDescriptor> readJCAMPldr(QString &first,QTextStream &in) ;
specModelItem* readJCAMPBlock(QTextStream& in) ;
void readJCAMPdata(QTextStream& in, QList<specDataPoint>& data, double step, double xfactor, double yfactor) ;
QVector<double> waveNumbers(QTextStream&) ;
QHash<QString,specDescriptor> fileHeader(QTextStream&) ;
QList<specModelItem*> (*fileFilter(QString& fileName)) (QFile&);
QList<double> gaussjinv(QList<QList<double> >&,QList<double>&) ;

bool comparePoints(const QPointF&, const QPointF&) ;

#endif
