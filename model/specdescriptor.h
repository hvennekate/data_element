#ifndef SPECDESCRIPTOR_H
#define SPECDESCRIPTOR_H
#include <QDataStream>
#include <QString>
#include <names.h>

struct specDescriptor;

QDataStream& operator<<(QDataStream&, const specDescriptor&);
QDataStream& operator>>(QDataStream&, specDescriptor&);

class specDescriptor{ //make inline
private:
	QString contentValue ;
	QString currentLine ;
	spec::descriptorFlags properties ;
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
	
	specDescriptor& operator=(const double&) ;
	specDescriptor& operator=(const QString&) ;
	
	friend QDataStream& operator<<(QDataStream&, const specDescriptor&);
	friend QDataStream& operator>>(QDataStream&, specDescriptor&);
};

#endif
