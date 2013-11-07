#ifndef SPECTEXTMIMECONVERTER_H
#define SPECTEXTMIMECONVERTER_H
#include "specmimetextexporter.h"

class QTextStream ;

class specTextMimeConverter : public specMimeTextExporter
{
	Q_OBJECT
public:
	explicit specTextMimeConverter(QObject* parent = 0);
	virtual QList<specModelItem*> importData(const QMimeData* data) ;
	bool canImport(const QStringList& types) ;
};

#endif // SPECTEXTMIMECONVERTER_H
