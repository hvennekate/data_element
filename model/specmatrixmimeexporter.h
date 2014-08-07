#ifndef SPECMATRIXMIMEEXPORTER_H
#define SPECMATRIXMIMEEXPORTER_H

#include "specmimeconverter.h"

class specMatrixMimeExporter : public specMimeConverter
{
	Q_OBJECT
	QString Descriptor;
	QString Separator ;
	bool XAcross ;
	bool SortY ;
public:
	explicit specMatrixMimeExporter(QObject *parent = 0);
	QList<specModelItem*> importData(const QMimeData *) ;
	QString matrix(QList<specModelItem*>&) ;
	void exportData(QList<specModelItem*>&, QMimeData*) ;
	void setDescriptor(const QString& d) ;
	void setXAcross(bool across = true) ;
	void setSeparator(const QString& s) ;
	void setSortY(bool sortY) ;
};

#endif // SPECMATRIXMIMEEXPORTER_H
