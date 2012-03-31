#ifndef SPECMIMECONVERTER_H
#define SPECMIMECONVERTER_H
#include "specmodelitem.h"

class specMimeConverter
{
public:
        specMimeConverter();
        virtual QList<specModelItem*> convert(QDataStream&) ;
        virtual QDataStream& convert(QList<specModelItem*>&, QDataStream &) ;
//        virtual QDataStream& convert(QList<specModelItem*>&, QMimeData*) ;
};

#endif // SPECMIMECONVERTER_H
