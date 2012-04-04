#include "speclogentryitem.h"


specLogEntryItem::specLogEntryItem ( QHash<QString,specDescriptor> desc, specFolderItem* par, QString tag)
 : specModelItem(par, tag), description(desc)
{
}


specLogEntryItem::~specLogEntryItem()
{
}


bool specLogEntryItem::isEditable(QString key) const
{
	if (description.contains(key))
		return description[key].isEditable() ;
	
	return specModelItem::isEditable(key);
}

bool specLogEntryItem::changeDescriptor(QString key, QString value)
{
	if (key=="")
		return specModelItem::changeDescriptor(key,value) ;
	if (description.contains(key))
		return description[key].setContent(value) ;
	return specModelItem::changeDescriptor(key,value) ;
}

QStringList specLogEntryItem::descriptorKeys() const
{
//	return (specModelItem::descriptorKeys() << specFolderItem::descriptorKeys() << description.keys()) ;
	return (specModelItem::descriptorKeys() << description.keys()) ;
}

QIcon specLogEntryItem::decoration() const { return QIcon(":/log_message.png") ; }

spec::descriptorFlags specLogEntryItem::descriptorProperties(const QString& key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
//	if (!description.contains(key)) return specFolderItem::descriptorProperties(key) ;
	return description[key].flags() ;
}

QString specLogEntryItem::descriptor(const QString &key, bool full) const
{
	if (key == "")
		return specModelItem::descriptor(key) ;
	if (description.contains(key))
		return description[key].content(full) ;
	return specModelItem::descriptor(key) ;
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
	return in ; // >> folderType ;
//	return specModelItem::readFromStream(in) ;
}

QDataStream& specLogEntryItem::writeToStream ( QDataStream& out) const
{
	qDebug("writing log entry") ;
	out << (quint8) spec::logEntry << description.size() ;
	for (QHash<QString,specDescriptor>::size_type i = 0 ; i < description.size() ; i++)
		out << (description.keys()[i]) << (description[description.keys()[i]]) ;
//	return specModelItem::writeToStream(out) ;
	return out ;
}
