#include "spectextmimeconverter.h"
#include <QTextStream>
#include "specmodelitem.h"
#include <QMimeData>
#include "specfolderitem.h"
#include "specdataitem.h"
#include <QDoubleValidator>

specTextMimeConverter::specTextMimeConverter(QObject *parent)
	: specMimeTextExporter(parent)
{
}

QList<specModelItem*> specTextMimeConverter::importData(const QMimeData *data)
{
	QStringList lines = data->text().split("\n") ;
	QDoubleValidator validator ;
	QVector<specDataPoint> dataPoints ;
	foreach(QString line, lines)
	{
		QStringList values = line.split(QRegExp("\\s+")) ;
		if (values.size() < 2) continue ;
		int pos = 0 ;
		if (validator.validate(values[0],pos) != QValidator::Acceptable) continue ;
		pos = 0 ; // TODO check if changed!
		if (validator.validate(values[1],pos) != QValidator::Acceptable) continue ;
		dataPoints << specDataPoint(0,values[0].toDouble(), values[1].toDouble(),0) ;
	}
	if (dataPoints.isEmpty()) return QList<specModelItem*>() ;
	return QList<specModelItem*>() << new specDataItem(dataPoints, QHash<QString, specDescriptor>()) ;
}

bool specTextMimeConverter::canImport(const QStringList &types)
{
	return types.contains("text/plain") ;
}
