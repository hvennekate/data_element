#include "specdescriptor.h"
#include <QRegExp>
#include <QStringList>
#include <QVariant>
#include "utility-functions.h"
#include <cmath>

void specDescriptor::writeToStream(QDataStream& out) const
{
	out << contentValue
	    << currentLine
	    << (quint8) multiline ;
}

void specDescriptor::readFromStream(QDataStream& in)
{
	quint8 ml ;
	in >> contentValue >> currentLine >> ml ;
	multiline = ml ;
}

bool specDescriptor::alternativeType(specStreamable::type t) const
{
	return specStreamable::legacyDescriptor == t ;
}

void specDescriptor::initializeFromString(const QString &c, quint32 l, bool ml)
{
	setContent(c) ;
	setActiveLine(l) ;
	setMultiline(ml);
}

bool specDescriptor::bothNumeric(const specDescriptor &other) const
{
	return isNumeric() && other.isNumeric() ;
}

bool specDescriptor::isNumeric() const
{
	return contentValue.type() == QVariant::Double ;
}

specDescriptor::specDescriptor(QString cont, quint32 cl, bool ml)
{
	initializeFromString(cont, cl, ml) ;
}

specDescriptor::specDescriptor(const QStringList &l , quint32 cl, bool ml)
{
	initializeFromString(l.join("\n"), cl, ml);
}


specDescriptor::specDescriptor(double d)
	: contentValue(d),
	  currentLine(0),
	  multiline(false)
{}

double specDescriptor::numericValue() const
{
	return contentValue.toDouble() ;
}

QString specDescriptor::content(bool full) const
{
	if(full || isMultiline())
		return contentValue.toString() ;
	return contentValue.toString().split("\n") [qAbs(currentLine)] ;
}

void specDescriptor::setContent(const QString& string)
{
	contentValue = string ;
	setActiveLine(activeLine());
}

void specDescriptor::setContent(const QStringList &l)
{
	setContent(l.join("\n"));
}

void specDescriptor::setActiveLine(quint32 line)
{
	currentLine = qBound((quint32) 0, line, (quint32) contentValue.toString().count("\n")) ;
}

void specDescriptor::setMultiline(bool on)
{
	multiline = on ;
}

void specDescriptor::setAppropriateContent(const QString &s)
{
	bool ok = false ;
	setContent(s.toDouble(&ok)) ;
	if (!ok)
		setContent(s) ;
}

quint32 specDescriptor::activeLine() const
{
	return currentLine ;
}

bool specDescriptor::isMultiline() const
{
	return multiline ;
}

void specDescriptor::setContent(const double& number)
{
	contentValue = number ;
}

specDescriptor& specDescriptor::operator= (const double& val)
{
	setContent(val) ;
	return *this ;
}


specDescriptor& specDescriptor::operator= (const QString& val)
{
	setContent(val);
	return *this ;
}

bool specDescriptor::operator==(const specDescriptor &other) const
{
	if (bothNumeric(other))
		return doubleComparison(numericValue(),
					other.numericValue()) ;
	return other.contentValue == contentValue ;
}

bool specDescriptor::fuzzyComparison(const specDescriptor &other, double tolerance) const
{
	if (0 == tolerance
			|| !bothNumeric(other))
		return *this == other ;
	if (std::isnan(numericValue()) && std::isnan(other.numericValue()))
		return true ;
	return qAbs(this->numericValue() - other.numericValue()) <= tolerance ;
}

specDescriptor specDescriptor::merge(const QList<specDescriptor> &list)
{
	if (list.size() == 1) return list.first() ;
	QStringList result ;
	foreach(const specDescriptor& d, list)
		result << d.contentValue.toString() ;
	return result.join("\n") ;
}

specDescriptor &specDescriptor::operator+=(const specDescriptor &other)
{
	if (bothNumeric(other))
		setContent((numericValue() + other.numericValue())) ;
	else if (!content(true).contains(other.content(true)))
		setContent(QStringList()
			    << content(true)
			   << other.content(true));
	return (*this) ;
}

specDescriptor specDescriptor::operator+(const specDescriptor &other) const
{
	specDescriptor newDescriptor(*this) ;
	return newDescriptor += other ;
}

specDescriptor specDescriptor::operator*(const double &d) const
{
	specDescriptor newDescriptor(*this) ;
	if (isNumeric())
		newDescriptor.setContent(numericValue() *d) ;
	return newDescriptor ;
}

specDescriptor::operator QString() const
{
	return content(true) ;
}

void specDescriptor::readAlternative(QDataStream& in, type t)
{
	if (t != legacyDescriptor) return ;
	// Read in old descriptor data
	qint8 properties ;
	QString currentLineContent ;
	QString content ;
	in >> content >> currentLineContent >> properties ;

	// get content
	setContent(content) ;
	// get active line
	setActiveLine(content.left(contentValue.toString().indexOf(currentLineContent)).count("\n")) ;
	// Determine if all lines are to be shown
	setMultiline((qint8)4 & properties); // 4 = spec::multiline
}


void specLegacyDescriptor::readFromStream(QDataStream &in)
{
	readAlternative(in, myType);
	myType = specStreamable::descriptor ;
}

specLegacyDescriptor::specLegacyDescriptor()
	: specDescriptor(),
	  myType(specStreamable::legacyDescriptor)
{
}
