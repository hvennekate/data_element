#include "speclogtodataconverter.h"
#include "speclogentryitem.h"
#include "utility-functions.h"
#include <QStack>
#include <QMessageBox>
#include <QMimeData>
#include "speclogmodel.h"
#include <QSettings>
#include "speclogmessage.h"

specLogToDataConverter::specLogToDataConverter(QObject *parent)
	: specMimeConverter(parent)
{
}

specModelItem* specLogToDataConverter::factory(const type &t) const
{
	return specModelItem::itemFactory(t) ;
}

bool specLogToDataConverter::canImport(const QStringList &types)
{
	return qobject_cast<specModel*>(parent()) &&
	       !qobject_cast<specLogModel*>(parent()) &&
	       types.contains("application/spec.logged.files") ;
}

QStringList specLogToDataConverter::importableTypes() const
{
    return specMimeConverter::importableTypes() << "application/spec.logged.files" ;
}

void specLogToDataConverter::toStream(specModelItem *item, QDataStream & out)
{
	QPair<qint8, QString> entry ;
	if (item->isFolder())
	{
		entry.first = 1 ;
		entry.second = item->descriptor("",true) ;
		out << entry ;
		entry.first = 2 ;
		specFolderItem* folder = (specFolderItem*) item ;
		for (int i = 0 ; i < folder->children() ; ++i)
			toStream(folder->child(i),out) ;
	}
	else if (!dynamic_cast<specLogMessage*>(item))
	{
		entry.first = 0 ;
		entry.second = item->descriptor("Datei") ;
	}
	else return ;
	out << entry ;
}

void specLogToDataConverter::exportData(QList<specModelItem *> &items, QMimeData *data)
{
	QByteArray ba ;
	QDataStream out(&ba, QIODevice::WriteOnly) ;
	foreach (specModelItem* item, items)
		toStream(item, out) ;
	if (!ba.isEmpty())
		data->setData("application/spec.logged.files",ba) ;
}

QList<specModelItem*> specLogToDataConverter::importData(const QMimeData *data)
{
	QSettings settings ;
	QByteArray ba(data->data("application/spec.logged.files")) ;
	QDataStream in(&ba,QIODevice::ReadOnly) ;
	QPair<qint8, QString> entry ;
	QStack<specFolderItem*> parents ;
	QList<specModelItem*> items ;
	while (!in.atEnd())
	{
		in >> entry ;
		if (entry.first == 1)
		{
			specFolderItem *newItem = new specFolderItem(0,entry.second) ;
			if (parents.empty())
				items << newItem ;
			else
				parents.top()->addChild(newItem,parents.top()->children());
			parents.push(newItem) ;
		}
		else if (entry.first == 2)
			parents.pop() ;
		else
		{
			//if item is a log item, try to open corresponding file
			QString fileName = entry.second ;
			fileName = fileName.section("\\",-1,-1) ;

			QDir::setCurrent(currentDirectory.absolutePath()) ;
			QFile file(fileName) ;
			if (!file.exists())
			{
				QStringList directories(settings.value("logConverter/importDirectory").toStringList()) ;
				foreach(QString directory, directories)
				{
					currentDirectory.setPath(directory) ;
					QDir::setCurrent(currentDirectory.absolutePath()) ;
					file.setFileName(currentDirectory.absoluteFilePath(fileName)) ;
					if (file.exists()) break ;
				}

				if (!file.exists())
				{
					QFileDialog path ;
					path.setWindowTitle("Datei zum Importieren angeben") ;
					path.setDirectory(currentDirectory) ;
					path.setFileMode(QFileDialog::ExistingFile);
					path.selectFile(fileName) ;
					path.setNameFilters(QStringList(QString("%1 (%1)").arg(fileName))) ;
					if (path.exec() == QDialog::Rejected || path.selectedFiles().isEmpty())
					{
						if (QMessageBox::question(0,
									  "Import abbrechen?",
									  "Soll der Import von Datendateien abgebrochen werden?",
									  QMessageBox::Yes | QMessageBox::No,
									  QMessageBox::No) == QMessageBox::Yes)
						{
							foreach(specModelItem* item, items)
								delete item ;
							items.clear();
							return items ;
						}
						continue ;
					}
					currentDirectory = path.directory() ;
					directories << currentDirectory.absolutePath() ;
					settings.setValue("logConverter/importDirectory",directories) ;
					file.setFileName(path.selectedFiles().first()) ;
				}
			}
			if (!file.open(QFile::ReadOnly | QFile::Text))
			{
				QMessageBox::warning(0,
						     "Fehler",
						     "Kann Datei nicht oeffnen: " + fileName) ;
				continue ;
			}
			QList<specModelItem*> (*filter)(QFile& )  = fileFilter(file.fileName()) ;
			if (!filter)
			{
				QMessageBox::warning(0,
						     "Fehler",
						     "Kein gueltiger Dateityp:" + fileName) ;
				continue ;
			}
            specFolderItem* newItem = new specFolderItem ;
            newItem->changeDescriptor("",currentDirectory.absolutePath() + "\n"+ fileName) ;
            newItem->setActiveLine("",1) ;
			newItem->addChildren(filter(file)) ;
			if (parents.isEmpty())
				items << newItem ;
			else
				parents.top()->addChild(newItem,parents.top()->children());
		}
	}
	return items ;
}
