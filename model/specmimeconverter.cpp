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

bool specMimeConverter::canImport(const QStringList &)
{
	return false ;
}
