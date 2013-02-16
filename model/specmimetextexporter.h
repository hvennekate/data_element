#ifndef SPECMIMETEXTEXPORTER_H
#define SPECMIMETEXTEXPORTER_H
#include "specmimeconverter.h"

class QTextStream ;

class specMimeTextExporter : public specMimeConverter
{
	Q_OBJECT
private:
    void writeItem(specModelItem*, QTextStream& out) ;
public:
	explicit specMimeTextExporter(QObject *parent = 0);
	virtual QList<specModelItem*> importData(const QMimeData *data) ;
	void exportData(QList<specModelItem *> &, QMimeData *) ;
};

#endif // SPECMIMETEXTEXPORTER_H
