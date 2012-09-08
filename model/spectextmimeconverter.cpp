#include "spectextmimeconverter.h"
#include <QTextStream>
#include "specmodelitem.h"
#include <QMimeData>
#include "specfolderitem.h"
#include "specdataitem.h"
#include <QDoubleValidator>

specTextMimeConverter::specTextMimeConverter(QObject *parent)
	: specMimeConverter(parent)
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

void specTextMimeConverter::exportData(QList<specModelItem *> &items, QMimeData *data)
{
	QString text ;
	QTextStream out(&text, QIODevice::WriteOnly) ;
	foreach(specModelItem* item, items)
		writeItem(item, out) ;
	data->setText(text) ;
}

void specTextMimeConverter::writeItem(const specModelItem* item, QTextStream& out)
{
	foreach(QString descriptor, item->descriptorKeys())
	{
		QString padding(descriptor.length(),' ') ;
		QStringList content = (QString("# ") + descriptor + (descriptor == "" ? "" : ": ") +
				       item->descriptor(descriptor,true)).split("\n") ;
		for (QStringList::iterator i = content.begin()+1 ; i != content.end() ; ++i)
			i->prepend("#   " + padding) ;
		out << content.join("\n") << endl ;
	}
	if (item->isFolder())
	{
		for (int i = 0 ; i < item->children() ; ++i)
		{
			out << "# Child item" << endl ;
			writeItem(((specFolderItem*)item)->child(i),out) ;
		}
	}
	else
	{
		for (size_t i = 0 ; i < item->dataSize() ; ++i)
		{
			QPointF point = item->sample(i) ;
			out << point.x() << "\t" << point.y() << endl ;
		}
	}
	out << endl ;
}

bool specTextMimeConverter::canImport(const QStringList &types)
{
	return types.contains("text/plain") ;
}
