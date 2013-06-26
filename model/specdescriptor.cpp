#include "specdescriptor.h"
#include <QRegExp>
#include <QStringList>

void specDescriptor::writeToStream(QDataStream &out) const
{
	// TODO bad legacy...
	out << contentValue << contentValue.split("\n")[currentLine] << (qint8) properties ;
}

void specDescriptor::readFromStream(QDataStream &in)
{
	qint8 prop ;
	QString cline ;
	in >> contentValue >> cline >> prop ;
	currentLine = contentValue.left(contentValue.indexOf(cline)).count("\n") ;
	properties = (spec::descriptorFlags) prop ;
}

specDescriptor::specDescriptor(QString cont, spec::descriptorFlags prop)
	: currentLine(0),
	  properties(prop)
{
	(*this) = cont ;
}

specDescriptor::specDescriptor(double d)
	: currentLine(0),
	  properties(spec::numeric)
{
	(*this) = d ;
}

double specDescriptor::numericValue() const
{ return contentValue.toDouble() ; }

QString specDescriptor::content(bool full) const
{
	if (full || (properties & spec::multiline))
		return contentValue ;
	else
		return contentValue.split("\n")[currentLine] ;
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
	currentLine = qBound(0,line,contentValue.count("\n")) ;
	return true ;
}

int specDescriptor::activeLine() const
{
	return currentLine ;
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
{
	return properties & spec::numeric ;
}

bool specDescriptor::isEditable() const
{
	return properties & spec::editable ;
}

spec::descriptorFlags specDescriptor::flags() const
{
	return properties ;
}

specDescriptor& specDescriptor::operator=(const double& val)
{
	(*this) = QString::number(val) ;
	properties |= spec::numeric ;
	return *this ;
}


specDescriptor& specDescriptor::operator=(const QString& val)
{
	contentValue = val ;
	setActiveLine(currentLine) ;
	return *this ;
}

void specDescriptor::setFlags(spec::descriptorFlags f)
{
	properties = f ;
}
