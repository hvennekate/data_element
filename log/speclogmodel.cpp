#include "speclogmodel.h"
#include <QFile>
#include <QFileDialog>
#include "utility-functions.h"

specLogModel::specLogModel(QObject *parent)
	: specModel(parent)
{
}

QList<specModelItem*> specLogToData(const QMimeData* data)
{
	QByteArray encodedData = data->data("application/spec.log.items") ;
	QDataStream stream(&encodedData, QIODevice::ReadOnly) ;

	// TODO this is highly redundant with the specmodel implementation
	QList<specModelItem*> list ;
	while(!stream.atEnd())
	{
		specStreamable::type t ;
		stream >> t ;
		specModelItem *newItem = specModelItem::itemFactory(t) ;
		newItem->readDirectly(stream); ;
		list << newItem ;
	}

	// transform log entries to data entries
	QList<specModelItem*> toConvert = list;
	for (int i = 0 ; i < toConvert.size() ; ++i)
	{
		if (list[i]->isFolder())
		{
			specFolderItem *folder = (specFolderItem*) list.takeAt(i) ;
			for (int j = folder->children()-1 ; j >= 0 ; --j)
				toConvert.insert(i, folder->child(j)) ;
			--i ;
		}
	}

	for (int i = 0 ; i < toConvert.size() ; ++i)
	{
		QString filename = toConvert[i]->descriptor("Datei") ;
		filename = filename.section("\\",-1) ;
		if (!QFile::exists(filename))
			filename = QFileDialog::getOpenFileName() ;
		if (filename.isEmpty())
			continue ;

		QFile fileToRead(filename) ;
		QList<specModelItem*> (*importFunction)(QFile&) = fileFilter(filename) ;
		fileToRead.open(QFile::ReadOnly | QFile::Text) ;
		QList<specModelItem*> importedItems = importFunction(fileToRead) ;
		if (toConvert[i]->parent())
		{
			toConvert[i]->parent()->addChildren(importedItems,toConvert[i]->parent()->childNo(toConvert[i])) ;
			toConvert[i]->setParent(0) ;
		}
		else
		{
			for (int j = 0 ; j < importedItems.size() ; ++j)
				list.insert(list.indexOf(toConvert[i]),importedItems[j]) ;
			list.takeAt(list.indexOf(toConvert[i])) ;
		}

		delete toConvert[i] ;
	}

	return list ;
}
