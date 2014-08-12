#include "specmimetextexporter.h"
#include <QTextStream>
#include "specmodelitem.h"
#include "specfolderitem.h"
#include <QDoubleValidator>
#include <QMimeData>

specMimeTextExporter::specMimeTextExporter(QObject* parent) :
	specMimeConverter(parent)
{
}

void specMimeTextExporter::exportData(QList<specModelItem*>& items, QMimeData* data)
{
	QString text ;
	QTextStream out(&text, QIODevice::WriteOnly) ;
	foreach(specModelItem * item, items)
	writeItem(item, out) ;
	data->setText(text) ;
}

void specMimeTextExporter::writeItem(specModelItem* item, QTextStream& out)
{
	QList<QPair<int, QString> > dataTypeList ;
	dataTypeList << QPair<spec::value, QString> (spec::wavenumber, "\t")
		     << QPair<spec::value, QString> (spec::signal, "\t")
		     << QPair<spec::value, QString> (spec::maxInt, "\n") ;
	foreach(QString descriptor, item->descriptorKeys())
	{
		QString padding(descriptor.length(), ' ') ;
		QStringList content = (QString("# ") + descriptor + (descriptor == "" ? "" : ": ") +
				       item->descriptor(descriptor, true)).split("\n") ;
		for(QStringList::iterator i = content.begin() + 1 ; i != content.end() ; ++i)
			i->prepend("#   " + padding) ;
		out << content.join("\n") << endl ;
	}
	QStringList numericalDescriptors ;
	if(item->isFolder())
	{
		for(int i = 0 ; i < item->children() ; ++i)
		{
			out << "# Child item" << endl ;
			writeItem(((specFolderItem*) item)->child(i), out) ;
		}
	}
	else
		out << item->exportData(QList<QPair<bool, QString> >(), dataTypeList, numericalDescriptors);

	out << endl ;
}

QList<specModelItem*> specMimeTextExporter::importData(const QMimeData* data)
{
	Q_UNUSED(data) ;
	return QList<specModelItem*>() ;
}

