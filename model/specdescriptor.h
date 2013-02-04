#ifndef SPECDESCRIPTOR_H
#define SPECDESCRIPTOR_H
#include <QDataStream>
#include <QString>
#include <names.h>
#include "specstreamable.h"
#include <QVariant>

class specDescriptor : public specStreamable, private QVariant
{ //make inline
private:
	qint16 currentLine ;
	bool readOnly ;
	bool multiLine ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	specStreamable::type typeId() const { return specStreamable::descriptor ; }
public:
	specDescriptor(QString cont="", spec::descriptorFlags prop = spec::def) ;
	specDescriptor(double d) ;
	double numericValue() const ;
	QString content(bool full = false) const;
	bool setContent(const QString&) ;
	bool setActiveLine(int) ;
	bool setContent(const double&) ;
	bool isNumeric() const;
	bool isEditable() const;
	spec::descriptorFlags flags() const ;
	void setFlags(spec::descriptorFlags) ;
	int activeLine() const ;
	
	specDescriptor& operator=(const QString&) ;
};

QDataStream& operator<<(QDataStream& out, const specDescriptor&) ;
QDataStream& operator>>(QDataStream& in, specDescriptor&) ;

#endif
