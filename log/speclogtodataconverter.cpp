#include "speclogtodataconverter.h"
#include "speclogentryitem.h"
#include "utility-functions.h"

specLogToDataConverter::specLogToDataConverter()
{
}

specLogToDataConverter::~specLogToDataConverter()
{
}

QDataStream &specLogToDataConverter::convert(QList<specModelItem *> &list, QDataStream &stream)
{
	return specMimeConverter::convert(list,stream) ;
}

QList<specModelItem*> specLogToDataConverter::convert(QDataStream &stream)
{
	QList<specModelItem*> items = specMimeConverter::convert(stream) ;
	for (int i = 0 ; i < items.size() ; ++i)
	{
		specLogEntryItem *logItem = dynamic_cast<specLogEntryItem*>(items[i]) ;
		//item may be a folder
		if (!logItem)
			logItem = dynamic_cast<specFolderItem*>(items[i]) ;
		//item may possibly not be a log item...
		if (!logItem)
			delete items.takeAt(i--) ;
		else
		{
			specModelItem *newItem = getData(items[i]) ;
			if (newItem != items[i])
			{
				delete item[i] ;
				if (newItem)
					items[i] = newItem ;
				else
					items.takeAt(i --) ;
			}
		}
	}
	return items ;
}

specModelItem* specLogToDataConverter::getData(specModelItem *item)
{
	// item may be a folder
	if (item->isFolder())
	{
		specFolderItem *folder = (specFolderItem*) item ;
		QVector<specModelItem*> children ;
		for (int i = 0 ; i < item->children() ; ++i)
		{
			specModelItem* child = folder->child(i)
					specModelItem* newChild = getData(child) ;
			if (newChild)
			{
				folder->addChild(newChild,folder->childNo(child)) ;
				delete child ;
			}
		}
		return folder ;
	}


	//if item is a log item, try to open corresponding file
	QString fileName = item->descriptor("file",false) ;
	fileName = fileName.section("\\",-1,-1) ;

	QFileDialog path ;
	path.setWindowTitle("Datei zum Importieren angeben") ;
	path.setDirectory(currentDirectory) ;
	path.setFileMode(QFileDialog::ExistingFile);
	path.selectFile(fileName) ;
	path.setNameFilters(QStringList(fileName)) ;
	if (!path.exec())
		return 0 ;
	currentDirectory = path.directory() ;

	specFolderItem *newItem = new specFolderItem(0,fileName) ;

	if (path.selectedFiles().isEmpty())
		return 0 ;

	QFile file(path.selectedFiles().first()) ;
	newItem->addChildren(fileFilter(file)(file)) ; // TODO take care that there actually is a file filter...
}
