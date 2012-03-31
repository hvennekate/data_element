#include "speclogtodataconverter.h"
#include "speclogentryitem.h"

specLogToDataConverter::specLogToDataConverter()
{
    path = new QFileDialog ;
    path->setWindowTitle("Suchpfad angeben") ;
}

specLogToDataConverter::~specLogToDataConverter()
{
    delete path ;
}

QDataStream &specLogToDataConverter::convert(QDataStream &stream)
{
    QList<specModelItem*> items = specMimeConverter::convert(stream) ;

}

specModelItem* specLogToDataConverter::getData(specModelItem *item)
{
    if (item->isFolder())
    {
        specFolderItem *folder = (specFolderItem*) item ;
        QVector<specModelItem*> children ;
        for (int i = 0 ; i < item->children() ; ++i)
        {
            specModelItem* child = folder->child(i)
            specModelItem* newChild = getData(child) ;
            if (newChild)
                delete child ;
        }
        return 0 ;
    }
    specLogEntryItem *log = dynamic_cast<specLogEntryItem>(item) ;
    if (!log)
        return 0 ;
}
