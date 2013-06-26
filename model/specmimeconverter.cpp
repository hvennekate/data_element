#include "specmimeconverter.h"
#include <QMimeData>
#include <QStringList>

specMimeConverter::specMimeConverter(QObject *parent)
	: QObject(parent)
{
}

bool specMimeConverter::canImport(const QMimeData *data)
{
	return canImport(data->formats()) ;
}

bool specMimeConverter::canImport(const QStringList &types)
{
	QStringList importable = importableTypes() ;
	foreach(const QString& type, types)
		if (importable.contains(type)) return true ;
	return false ;
}

QStringList specMimeConverter::importableTypes() const
{
	return QStringList() ;
}
