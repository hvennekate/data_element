#ifndef SPECMIMEFILEIMPORTER_H
#define SPECMIMEFILEIMPORTER_H

#include "specmimeconverter.h"
#include "names.h"

class specMimeFileImporter : public specMimeConverter
{
	Q_OBJECT
private:
public:
	explicit specMimeFileImporter(QObject* parent = 0);
	QList<specModelItem*> importData(const QMimeData*) ;
	void exportData(QList<specModelItem*>&, QMimeData*) ;
	QStringList importableTypes() const ;
signals:

public slots:

};

#endif // SPECMIMEFILEIMPORTER_H
