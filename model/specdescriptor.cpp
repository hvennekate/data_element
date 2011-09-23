#include "specdescriptor.h"
#include <QRegExp>
#include <QDebug>

QDataStream& operator<<(QDataStream& out, const specDescriptor& desc)
{
	out << desc.contentValue << (int) desc.properties ;
	if (desc.currentLine != QString())
		out << desc.currentLine ;
	qDebug("out desc props: %d %d",(int)desc.properties, desc.isEditable()) ;
	return out ;
}

QDataStream& operator>>(QDataStream& in , specDescriptor& desc) 
{
	int prop ;
	in  >> desc.contentValue >> prop ;
	desc.properties = (spec::descriptorFlags) prop ;
	qDebug("in desc props: %d %d",(int) desc.properties, desc.isEditable()) ;
	if (desc.contentValue.contains(QRegExp("\n")))
		in >> desc.currentLine ;
	return in ;
}


specDescriptor::specDescriptor(QString cont, spec::descriptorFlags prop)
	: currentLine(QString()),
	  properties(prop)
{
	(*this) = cont ;
}

specDescriptor::specDescriptor(double d)
	: currentLine(QString()),
	  properties(spec::numeric)
{
	(*this) = d ;
}

double specDescriptor::numericValue() const
{ return contentValue.toDouble() ; }

QString specDescriptor::content() const
{
	if (currentLine == QString())
		return contentValue ;
	else
		return currentLine ;
}

bool specDescriptor::setContent(const QString& string)
{
	if (isEditable())
	{
		(*this) = string ;
		return true ;
	}
	return false ;
}

bool specDescriptor::setContent(const double& number)
{
	if (isNumeric() && isEditable())
	{
		(*this) = number ;
		return true ;
	}
	return false ;
}

bool specDescriptor::isNumeric() const
{ return properties & spec::numeric ; }

bool specDescriptor::isEditable() const
{ return properties & spec::editable ; }

spec::descriptorFlags specDescriptor::flags() const
{ return properties ;}

specDescriptor& specDescriptor::operator=(const double& val)
{
	(*this) = QString("%1").arg(val) ;
	properties |= spec::numeric ;
	return *this ;
}

specDescriptor& specDescriptor::operator=(const QString& val)
{
	contentValue = val ;
	if (val.contains(QRegExp("\n")))
		currentLine = val.section(QRegExp("\n"),0,0) ;
	else
		currentLine = QString() ;
	qDebug() << "----------Multiline:" << val.contains(QRegExp("\n")) << currentLine ;

	qDebug() << "------------desc: " << val << currentLine ;
	return *this ;
}
