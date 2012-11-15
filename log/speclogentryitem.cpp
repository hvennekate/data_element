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
    if (description.contains(key)) return description[key].isEditable() ;
	return specModelItem::isEditable(key);
}

bool specLogEntryItem::isNumeric(const QString &key) const
{
    if (description.contains(key)) return description[key].isNumeric() ;
    return specModelItem::isNumeric(key);
}

bool specLogEntryItem::changeDescriptor(QString key, QString value)
{
	if (description.contains(key))
		return description[key].setContent(value) ;
    if (specModelItem::descriptorKeys().contains(key))
        return specModelItem::changeDescriptor(key,value) ;
    description[key] = specDescriptor(value,spec::editable) ;
    return true ;
}

QStringList specLogEntryItem::descriptorKeys() const
{
	return (specModelItem::descriptorKeys() << description.keys()) ;
}

QIcon specLogEntryItem::decoration() const { return QIcon(":/log_message.png") ; }

spec::descriptorFlags specLogEntryItem::descriptorProperties(const QString& key) const
{
    if (description.contains(key)) return description[key].flags() ;
    return specModelItem::descriptorProperties(key) ;
}

void specLogEntryItem::setDescriptorProperties(const QString &key, spec::descriptorFlags f)
{
    if (description.contains(key)) description[key].setFlags(f) ;
    else specModelItem::setDescriptorProperties(key, f) ;
}

QString specLogEntryItem::descriptor(const QString &key, bool full) const
{
    if (description.contains(key)) return description[key].content(full) ;
    return specModelItem::descriptor(key, full) ;
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

bool specLogEntryItem::setActiveLine(const QString& key, int line)
{
    if (description.contains(key)) return description[key].setActiveLine(line) ;
    return specModelItem::setActiveLine(key,line) ;
}

int specLogEntryItem::activeLine(const QString& key) const
{
    if (description.contains(key)) return description[key].activeLine() ;
    return specModelItem::activeLine(key) ;
}

specLogEntryItem::specLogEntryItem(const specLogEntryItem & other)
    : specModelItem(other),
      description(other.description)
{}

double specLogEntryItem::numericalValue(const QString &key) const
{
    if (description.contains(key)) return description[key].numericValue() ;
    return NAN ;
}
