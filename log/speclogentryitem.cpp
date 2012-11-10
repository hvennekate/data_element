#include "speclogentryitem.h"


specLogEntryItem::specLogEntryItem ( QHash<QString,specDescriptor> desc, specFolderItem* par, QString tag)
	: specModelItem(par, tag),
	  description(desc)
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

void specLogEntryItem::setDescriptorProperties(const QString &key, spec::descriptorFlags f)
{
    if (description.contains(key)) description[key].setFlags(f) ;
    else specModelItem::setDescriptorProperties(key, f) ;
}

QString specLogEntryItem::descriptor(const QString &key, bool full) const
{
	if (key == "")
		return specModelItem::descriptor(key) ;
	if (description.contains(key))
		return description[key].content(full) ;
	return specModelItem::descriptor(key) ;
}

void specLogEntryItem::readFromStream ( QDataStream& in)
{
	specModelItem::readFromStream(in) ;
	in >> description ;
}

void specLogEntryItem::writeToStream ( QDataStream& out) const
{
	specModelItem::writeToStream(out) ;
	out << description ;
}
