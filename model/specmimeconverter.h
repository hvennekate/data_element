#ifndef SPECMIMECONVERTER_H
#define SPECMIMECONVERTER_H
#include <QObject>

class QMimeData ;
class specModelItem ;

class specMimeConverter : public QObject
{
	Q_OBJECT
public:
	explicit specMimeConverter(QObject* parent = 0) ;
	virtual QList<specModelItem*> importData(const QMimeData*) = 0 ;
	virtual void exportData(QList<specModelItem*>&, QMimeData*) = 0 ;
	virtual bool canImport(const QStringList&) ;
	virtual bool canImport(const QMimeData*) ;
    virtual QStringList importableTypes() const ;
};

#endif // SPECMIMECONVERTER_H
