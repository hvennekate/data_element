#include "speclogentryitem.h"


specLogEntryItem::specLogEntryItem(QHash<QString, specDescriptor> desc, specFolderItem* par, QString tag)
	: specModelItem(par, tag),
	  description(desc)
{
}


specLogEntryItem::~specLogEntryItem()
{
}

void specLogEntryItem::changeDescriptor(const QString &key, QString value)
{
	if(specModelItem::descriptorKeys().contains(key))
		specModelItem::changeDescriptor(key, value) ;
	else
		description[key].setAppropriateContent(value) ;
}

QStringList specLogEntryItem::descriptorKeys() const
{
	return (specModelItem::descriptorKeys() << description.keys()) ;
}

QIcon specLogEntryItem::decoration() const { return QIcon(":/logs.png") ; }


specDescriptor specLogEntryItem::getDescriptor(const QString &key) const
{
	if (description.contains(key)) return description[key] ;
	return specModelItem::getDescriptor(key);
}

void specLogEntryItem::readFromStream(QDataStream& in)
{
	specModelItem::readFromStream(in) ;
	in >> description ;
}

void specLogEntryItem::writeToStream(QDataStream& out) const
{
	specModelItem::writeToStream(out) ;
	out << description ;
}

void specLogEntryItem::setActiveLine(const QString& key, quint32 line)
{
	if(description.contains(key))
		description[key].setActiveLine(line) ;
	else
		specModelItem::setActiveLine(key, line) ;
}

void specLogEntryItem::setMultiline(const QString & key, bool on)
{
	if (description.contains(key))
		description[key].setMultiline(on) ;
	else
		specModelItem::setMultiline(key, on) ;
}

specLogEntryItem::specLogEntryItem(const specLogEntryItem& other)
	: specModelItem(other),
	  description(other.description)
{}

void specLogEntryItem::renameDescriptors(const QMap<QString, QString>& map)
{
	QHash<QString, specDescriptor> newDescription ;
	foreach(const QString & key, map.keys())
	newDescription[map[key]] = description[key] ;
	foreach(const QString & key, description.keys())
	if(!map.contains(key))
		newDescription[key] = description[key] ;
	description.swap(newDescription) ;
}

void specLogEntryItem::deleteDescriptor(const QString& key)
{
	description.remove(key) ;
}

void specLogEntryItem::dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const
{
	if(description.contains(key))
		destination << description[key] ;
	else
		specModelItem::dumpDescriptor(destination, key) ;
}

void specLogEntryItem::restoreDescriptor(QListIterator<specDescriptor>& origin, const QString& key)
{
	if(!origin.hasNext()) return ;
	if(key == "") specModelItem::restoreDescriptor(origin, key) ;
	else description[key] = origin.next() ;
}
