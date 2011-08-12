#include "specdescriptor.h"

QDataStream& operator<<(QDataStream& out, const specDescriptor& desc)
{
	out << desc.contentValue << (int) desc.properties ;
	qDebug("out desc props: %d %d",(int)desc.properties, desc.isEditable()) ;
	return out ;
}

QDataStream& operator>>(QDataStream& in , specDescriptor& desc) 
{
	int prop ;
	in  >> desc.contentValue >> prop ;
	desc.properties = (spec::descriptorFlags) prop ;
	qDebug("in desc props: %d %d",(int) desc.properties, desc.isEditable()) ;
	return in ;
}


specDescriptor::specDescriptor(QString cont, spec::descriptorFlags prop)
{ properties = prop ; contentValue = cont ; }

specDescriptor::specDescriptor(double d)
{ properties = spec::numeric ; contentValue.setNum(d) ;}

double specDescriptor::numericValue() const
{ return contentValue.toDouble() ; }

QString specDescriptor::content() const
{ return contentValue ; }

bool specDescriptor::setContent(const QString& string)
{
	if (isEditable())
	{
		contentValue = string ;
		return true ;
	}
	return false ;
}

bool specDescriptor::setContent(const double& number)
{
	if (isNumeric() && isEditable())
	{
		contentValue.setNum(number) ;
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
	contentValue.setNum(val) ;
//	setContent(val) ;
	return *this ;
}

specDescriptor& specDescriptor::operator=(const QString& val)
{
	contentValue = val ;
//	setContent(val) ;
	return *this ;
}
