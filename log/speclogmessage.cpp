#include "speclogmessage.h"

specLogMessage::specLogMessage( QHash<QString,specDescriptor> desc, specFolderItem* par, QString tag)
	: specDataItem(QVector<specDataPoint>(),desc,par, tag)
{
}


specLogMessage::~specLogMessage()
{
}


bool specLogMessage::changeDescriptor ( QString key, QString value )
{ return key == "" ? specDataItem::changeDescriptor(key,value) : false ; }

bool specLogMessage::isEditable ( QString key ) const
{ return key == "" ? specDataItem::isEditable (key) : false ; }

QDataStream& specLogMessage::readFromStream ( QDataStream& in )
{
	quint8 dataType ;
	in >> dataType ;
	return specDataItem::readFromStream(in) ; }

QDataStream& specLogMessage::writeToStream ( QDataStream& stream ) const
{
// 	qDebug("writing log message item") ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "ID export:  " << (quint8) spec::data << endl ;
	stream << (quint8) spec::sysEntry ;
	return specDataItem::writeToStream(stream) ;
}

QIcon specLogMessage::decoration() const { return QIcon(":/sys_message.png") ; }

QMultiMap< double, QPair<double,double> >* specLogMessage::kinetics ( QList< specKineticRange * > arg1 ) const
{ return new QMultiMap<double,QPair<double,double> > ; }

specDataItem& specLogMessage::operator += ( const specDataItem& toAdd )
{ return *this ; }

spec::descriptorFlags specLogMessage::descriptorProperties ( const QString& key ) const
{ 	return key == "" ? specDataItem::descriptorProperties (key) : spec::def ; }

void specLogMessage::exportData ( const QList< QPair < bool , QString > >& arg1, const QList< QPair < spec :: value , QString > >& arg2, QTextStream& arg3 )
{
	//TODO
}
