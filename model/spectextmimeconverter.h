#ifndef SPECTEXTMIMECONVERTER_H
#define SPECTEXTMIMECONVERTER_H
#include "specmimeconverter.h"

class QTextStream ;

class specTextMimeConverter : public specMimeConverter
{
	Q_OBJECT
private:
	void writeItem(const specModelItem*, QTextStream& out) ;
public:
	explicit specTextMimeConverter(QObject* parent = 0);
	virtual QList<specModelItem*> importData(const QMimeData *data) ;
	void exportData(QList<specModelItem *> &, QMimeData *) ;
	bool canImport(const QStringList & types) ;
};

#endif // SPECTEXTMIMECONVERTER_H
