#ifndef SPECDESCRIPTOR_H
#define SPECDESCRIPTOR_H
#include <QDataStream>
#include <QString>
#include <names.h>
#include "specstreamable.h"

class specDescriptor : public specStreamable
{ //make inline
private:
	QString contentValue ;
	QString currentLine ;
	spec::descriptorFlags properties ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::descriptor ; }
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
	
	specDescriptor& operator=(const double&) ;
	specDescriptor& operator=(const QString&) ;

};

#endif
