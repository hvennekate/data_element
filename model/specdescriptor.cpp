#include "specdescriptor.h"
#include <QRegExp>
#include <QStringList>

void specDescriptor::writeToStream(QDataStream &out) const
{
	// TODO this is the legacy version.  Write the actual variant!
	out << content(true) << content(false) << (qint8) flags() ;
}

void specDescriptor::readFromStream(QDataStream &in)
{
	qint8 prop ;
	QString contentValue, cLine ;
	in  >> contentValue >> cLine >> prop ;
	readOnly = !((spec::descriptorFlags) prop & spec::editable);
	multiLine = ((spec::descriptorFlags) prop & spec::multiline) ;
	bool numeric = ((spec::descriptorFlags) prop & spec::numeric) ;
	if (numeric)
	{
		*this = contentValue.toDouble() ;
		currentLine = 0 ;
	}
	else
	{
		*this = contentValue ;
		currentLine = contentValue.left(contentValue.indexOf(cLine)).count("\n") ;
	}
}


specDescriptor::specDescriptor(QString cont, spec::descriptorFlags prop)
	: QVariant(),
	  currentLine(0),
	  readOnly(!(prop & spec::editable))
{
	*this = cont ;
}

specDescriptor::specDescriptor(double d)
	: QVariant(d),
	  currentLine(0),
	  readOnly(true)
{
}

double specDescriptor::numericValue() const
{
	return toDouble() ;
}

QString specDescriptor::content(bool full) const
{
	if (isNumeric()) return toString() ;
	if (canConvert(QVariant::StringList))
	{
		if (full || multiLine) return toStringList().join("\n") ;
		return toStringList()[currentLine] ;
	}
	return QString() ;
}

bool specDescriptor::setContent(const QString& string)
{
	if (readOnly) return false ;
	setValue(string.split("\n")) ;
	return true ;
}

bool specDescriptor::setActiveLine(int line)
{
	currentLine = qBound(0,line,toStringList().size()) ;
	return true ;
}

int specDescriptor::activeLine() const
{
	return currentLine ;
}

bool specDescriptor::setContent(const double& number)
{
	if (readOnly) return false ;
	if (!isNumeric()) return false ;
	setValue(number) ;
	return true ;
}

bool specDescriptor::isNumeric() const
{
	return canConvert(QVariant::Double) ;
}

bool specDescriptor::isEditable() const
{
	return !readOnly ;
}

spec::descriptorFlags specDescriptor::flags() const
{
	return (isNumeric() ? spec::numeric : spec::def) |
			(isEditable() ? spec::editable : spec::def) |
			(multiLine ? spec::multiline : spec::def) ;
}

void specDescriptor::setFlags(spec::descriptorFlags f)
{
	multiLine = f & spec::multiline ;
	readOnly = (!f & spec::editable) ;
	if (f & spec::numeric) convert(QVariant::Double) ;
	else convert(QVariant::StringList) ;
}

QDataStream& operator<<(QDataStream& out, const specDescriptor& d)
{
	return out << (const specStreamable&) d ;
}

QDataStream& operator>>(QDataStream& in, specDescriptor& d)
{
	return in >> (specStreamable&) d ;
}

specDescriptor& specDescriptor::operator =(const QString& s)
{
	setValue(s.split("\n")) ;
	return *this ;
}
