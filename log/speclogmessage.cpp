#include "speclogmessage.h"

specLogMessage::specLogMessage(specFolderItem* par, QString tag)
	: specModelItem(par,tag)
{
}


specLogMessage::~specLogMessage()
{
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

