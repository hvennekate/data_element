#include "speclogmessage.h"

specLogMessage::specLogMessage(QHash<QString,specDescriptor> description, specFolderItem* par, QString tag)
	: specLogEntryItem(description, par, tag)
{
	qDebug() << "Log string:" << tag ;
}


specLogMessage::~specLogMessage()
{
}

bool specLogMessage::isEditable ( QString key ) const
{
	Q_UNUSED(key)
	return false ;
}

QIcon specLogMessage::decoration() const { return QIcon(":/sys_message.png") ; }

spec::descriptorFlags specLogMessage::descriptorProperties ( const QString& key ) const
{
	Q_UNUSED(key)
	return spec::def ;
}

bool specLogMessage::changeDescriptor( QString key, QString value )
{
	Q_UNUSED(key)
	Q_UNUSED(value)
	return false ;
}
