#include "speclogentryitem.h"


specLogEntryItem::specLogEntryItem ( QHash<QString,specDescriptor> desc, specFolderItem* par, QString tag)
 : specFolderItem(par, tag), description(desc)
{
}


specLogEntryItem::~specLogEntryItem()
{
}


bool specLogEntryItem::isEditable(QString key) const
{
	if (description.contains(key))
		return description[key].isEditable() ;
	
	return specFolderItem::isEditable(key);
}

bool specLogEntryItem::changeDescriptor(QString key, QString value)
{
	if (key=="")
		return specModelItem::changeDescriptor(key,value) ;
	if (description.contains(key))
		return description[key].setContent(value) ;
	return specFolderItem::changeDescriptor(key,value) ;
}

QStringList specLogEntryItem::descriptorKeys() const
{
	return (specModelItem::descriptorKeys() << specFolderItem::descriptorKeys() << description.keys()) ;
}

QIcon specLogEntryItem::decoration() const { return QIcon(":/log_message.png") ; }

spec::descriptorFlags specLogEntryItem::descriptorProperties(const QString& key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	if (!description.contains(key)) return specFolderItem::descriptorProperties(key) ;
	return description[key].flags() ;
}

QString specLogEntryItem::descriptor(const QString &key, bool full) const
{
	if (key == "")
		return specModelItem::descriptor(key) ;
	if (description.contains(key))
	{
		qDebug() << "##### returning descriptor: " << description[key].content() ;
		return description[key].content(full) ;
	}
	return specFolderItem::descriptor(key) ;
}

QDataStream& specLogEntryItem::readFromStream ( QDataStream& in)
{
	quint8 folderType ;
	int descriptionSize ;
	in >> descriptionSize ;
	for (QHash<QString,specDescriptor>::size_type i = 0 ; i < descriptionSize ; i++)
	{
		QString key;
		in >> key ;
		in >> description[key] ;
	}
	in >> folderType ;
	return specFolderItem::readFromStream(in) ;
}

QDataStream& specLogEntryItem::writeToStream ( QDataStream& out) const
{
	qDebug("writing log entry") ;
	out << (quint8) spec::logEntry << description.size() ;
	for (QHash<QString,specDescriptor>::size_type i = 0 ; i < description.size() ; i++)
		out << (description.keys()[i]) << (description[description.keys()[i]]) ;
	return specFolderItem::writeToStream(out) ;
}
