#include "specdescriptor.h"
#include <QRegExp>
#include <QDebug>

QDataStream& operator<<(QDataStream& out, const specDescriptor& desc)
{
	out << desc.contentValue << desc.currentLine << (int) desc.properties ;
	qDebug("out desc props: %d %d",(int)desc.properties, desc.isEditable()) ;
	return out ;
}

QDataStream& operator>>(QDataStream& in , specDescriptor& desc) 
{
	int prop ;
	in  >> desc.contentValue >> desc.currentLine >> prop ;
	desc.properties = (spec::descriptorFlags) prop ;
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

QString specDescriptor::content(bool full) const
{
	if (currentLine == QString() || full )
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

bool specDescriptor::setActiveLine(int line)
{
	int lineCount = 0 ;
	int lastNewLine = 0 ;
	while ((lastNewLine = 1+ contentValue.indexOf(QRegExp("\n"),lastNewLine) ))
		lineCount ++ ;
	if (line > lineCount)
		return false ;
	currentLine = contentValue.section(QRegExp("\n"),line,line) ;
	return true ;
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
	currentLine = val.contains(QRegExp("\n")) ? val.section(QRegExp("\n"),0,0) : val ;
	return *this ;
}
