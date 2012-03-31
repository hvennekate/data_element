#ifndef SPECLOGTODATACONVERTER_H
#define SPECLOGTODATACONVERTER_H
#include "specmimeconverter.h"
#include <QFileDialog>

class specLogToDataConverter : public specMimeConverter
{
private:
    specModelItem* getData(specModelItem*) ;
    QDir currentDirectory ;
public:
    specLogToDataConverter();
    ~specLogToDataConverter() ;
    QDataStream& convert(QList<specModelItem *> &, QDataStream &) ;
    QList<specModelItem*> convert(QDataStream&) ;
};

#endif // SPECLOGTODATACONVERTER_H
