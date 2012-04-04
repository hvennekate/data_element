#include "specmimeconverter.h"

specMimeConverter::specMimeConverter()
{
}

QList<specModelItem*> specMimeConverter::convert(QDataStream &stream)
{
    QList<specModelItem*> list ;
    while(!stream.atEnd())
    {
            qDebug("dropping an item") ;
            specModelItem* pointer ;
            stream >> pointer ;
            list << pointer ;
    }
    return list ;
}

QDataStream& specMimeConverter::convert(QList<specModelItem *> &list, QDataStream &stream)
{
    for (QList<specModelItem*>::iterator i = list.begin() ; i != list.end() ; ++i)
	(*i)->writeOut(stream) ;
    return stream ;
}
