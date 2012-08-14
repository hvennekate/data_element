#include "spectextmimeconverter.h"
#include <QTextStream>
#include "specmodelitem.h"
#include <QMimeData>
#include "specfolderitem.h"

specTextMimeConverter::specTextMimeConverter(QObject *parent)
	: specMimeConverter(parent)
{
}

QList<specModelItem*> specTextMimeConverter::importData(const QMimeData *data)
{
	Q_UNUSED(data)
	return QList<specModelItem*>() ;
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
