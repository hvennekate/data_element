#include "speclogtodataconverter.h"
#include "speclogentryitem.h"
#include "utility-functions.h"
#include <QStack>
#include <QMessageBox>
#include <QMimeData>
#include "speclogmodel.h"
#include <QSettings>
#include "speclogmessage.h"

specLogToDataConverter::specLogToDataConverter(QObject* parent)
	: specMimeConverter(parent)
{
}

specModelItem* specLogToDataConverter::factory(const type& t) const
{
	return specModelItem::itemFactory(t) ;
}

bool specLogToDataConverter::canImport(const QStringList& types)
{
	return qobject_cast<specModel*> (parent()) &&
	       !qobject_cast<specLogModel*> (parent()) &&
	       types.contains("application/spec.logged.files") ;
}

QStringList specLogToDataConverter::importableTypes() const
{
	return specMimeConverter::importableTypes() << "application/spec.logged.files" ;
}

void specLogToDataConverter::toStream(specModelItem* item, QDataStream& out)
{
	QPair<qint8, QString> entry ;
	if(item->isFolder())
	{
		entry.first = 1 ;
		entry.second = item->descriptor("", true) ;
		out << entry ;
		entry.first = 2 ;
		specFolderItem* folder = (specFolderItem*) item ;
		for(int i = 0 ; i < folder->children() ; ++i)
			toStream(folder->child(i), out) ;
	}
	else if(!dynamic_cast<specLogMessage*>(item))
	{
		entry.first = 0 ;
		entry.second = item->descriptor("Datei") ;
	}
	else return ;
	out << entry ;
}

void specLogToDataConverter::exportData(QList<specModelItem*>& items, QMimeData* data)
{
	QByteArray ba ;
	QDataStream out(&ba, QIODevice::WriteOnly) ;
	foreach(specModelItem * item, items)
	toStream(item, out) ;
	if(!ba.isEmpty())
		data->setData("application/spec.logged.files", ba) ;
}

QStringList specLogToDataConverter::searchDirectories()
{
	QSettings settings ;
	return settings.value("logConverter/importDirectory").toStringList() ;
}

void specLogToDataConverter::setSearchDirectories(const QStringList &dirs)
{
	QSettings settings ;
	settings.setValue("logConverter/importDirectory", dirs) ;
}

QList<specModelItem*> specLogToDataConverter::importData(const QMimeData* data)
{
	QByteArray ba(data->data("application/spec.logged.files")) ;
	QDataStream in(&ba, QIODevice::ReadOnly) ;
	QPair<qint8, QString> entry ;
	QStack<specFolderItem*> parents ;
	QList<specModelItem*> items ;
	while(!in.atEnd())
	{
		in >> entry ;
		if(entry.first == 1)
		{
			specFolderItem* newItem = new specFolderItem(0, entry.second) ;
			if(parents.empty())
				items << newItem ;
			else
				parents.top()->addChild(newItem, parents.top()->children());
			parents.push(newItem) ;
		}
		else if(entry.first == 2)
			parents.pop() ;
		else
		{
			//if item is a log item, try to open corresponding file
			QString fileName = entry.second ;
			fileName = fileName.section("\\", -1, -1) ;

			QDir::setCurrent(currentDirectory.absolutePath()) ;
			QFile file(fileName) ;
			if(!file.exists())
			{
				foreach(QString directory, searchDirectories())
				{
					currentDirectory.setPath(directory) ;
					QDir::setCurrent(currentDirectory.absolutePath()) ;
					file.setFileName(currentDirectory.absoluteFilePath(fileName)) ;
					if(file.exists()) break ;
				}

				if(!file.exists())
				{
					QFileDialog path ;
					path.setWindowTitle(tr("Find file to import")) ;
					path.setDirectory(currentDirectory) ;
					path.setFileMode(QFileDialog::ExistingFile);
					path.selectFile(fileName) ;
					path.setNameFilters(QStringList(QString("%1 (%1)").arg(fileName))) ;
					if(path.exec() == QDialog::Rejected || path.selectedFiles().isEmpty())
					{
						if(!in.atEnd()
								&& QMessageBox::question(0,
									 tr("Cancel importing?"),
									 tr("Cancel the import of pending files?"),
									 QMessageBox::Yes | QMessageBox::No,
									 QMessageBox::No) == QMessageBox::Yes)
						{
//							foreach(specModelItem * item, items)
//								delete item ;
//							items.clear();
							return items ;
						}
						continue ;
					}
					currentDirectory = path.directory() ;
					setSearchDirectories(searchDirectories()
							     << currentDirectory.absolutePath()) ;
					file.setFileName(path.selectedFiles().first()) ;
				}
			}
			if(!file.open(QFile::ReadOnly | QFile::Text))
			{
				QMessageBox::warning(0,
						     tr("Error"),
						     tr("Cannot open this file: ") + fileName) ;
				continue ;
			}
			// TODO check if this works under windows... (opening an open file again...)
			specFileImportFunction filter = fileFilter(file.fileName()) ;
			if(!filter)
			{
				QMessageBox::warning(0,
						     tr("Error"),
						     tr("Not a valid file type: ") + fileName) ;
				continue ;
			}
			specFolderItem* newItem = new specFolderItem ;
			newItem->changeDescriptor("", currentDirectory.absolutePath() + "\n" + fileName) ;
			newItem->setActiveLine("", 1) ;
			newItem->addChildren(filter(file)) ;
			if(parents.isEmpty())
				items << newItem ;
			else
				parents.top()->addChild(newItem, parents.top()->children());
		}
	}
	return items ;
}
