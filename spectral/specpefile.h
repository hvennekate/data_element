#ifndef SPECPEFILE_H
#define SPECPEFILE_H

#include <QMap>
#include <QHash>
#include <QString>

class QByteArray ;
class specDataItem ;
class QVariant ;
class QString ;

class specPEFile
{
private:
    typedef QMap<int, QString> labelMapType ;
    typedef qint32 blockSize ;
    typedef qint16 innerCode ;
    typedef qint16 innerLength ;

    template<typename t> static t readInt(const QByteArray& ba, int pos = 0) ;
    static double readDouble(const QByteArray& ba, int pos = 0) ;

    labelMapType* labelMap ;

    QMap<QString, QVariant> data ;
    QString Description ;
    static QVariant readEntry(const QByteArray&, innerCode code) ;

    QByteArray readItem(const QByteArray&, QString label = QString()) ;
    void readFromArray(const QByteArray&) ;
public:
    specPEFile(const QByteArray&);
    operator specDataItem() const;
};

#endif // SPECPEFILE_H
