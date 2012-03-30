#ifndef SPECLOGTODATACONVERTER_H
#define SPECLOGTODATACONVERTER_H
#include "specmimeconverter.h"
#include <QFileDialog>

class specLogToDataConverter : public specMimeConverter
{
private:
    specModelItem* getData(specModelItem*) ;
    QFileDialog *path ;
public:
    specLogToDataConverter();
    ~specLogToDataConverter() ;
    QDataStream& convert(QDataStream &) ;
};

#endif // SPECLOGTODATACONVERTER_H
