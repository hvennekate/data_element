#include "speclogmessage.h"

specLogMessage::specLogMessage( QHash<QString,specDescriptor> desc, specFolderItem* par, QString tag)
	: specDataItem(QVector<specDataPoint>(),desc,par, tag)
{
}


specLogMessage::~specLogMessage()
{
}


bool specLogMessage::changeDescriptor ( QString key, QString value )
{
	return false ;
}

bool specLogMessage::isEditable ( QString key ) const
{
	return false ;
}

QIcon specLogMessage::decoration() const { return QIcon(":/sys_message.png") ; }

spec::descriptorFlags specLogMessage::descriptorProperties ( const QString& key ) const
{
	return spec::def ;
}

