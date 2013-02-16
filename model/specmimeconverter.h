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
    /// Default implementation:  check if any importable types are contained in list of types
	virtual bool canImport(const QStringList&) ;
    /// Default implementation: call canImport() on types of QMimeData object
	virtual bool canImport(const QMimeData*) ;
    /// Default: none (preferrably change this instead of canImport()
    virtual QStringList importableTypes() const ;
};

#endif // SPECMIMECONVERTER_H
