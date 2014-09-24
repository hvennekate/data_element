#ifndef SPECDESCRIPTOR_H
#define SPECDESCRIPTOR_H
#include <QDataStream>
#include <QString>
#include <names.h>
#include <QVariant>
#include "specstreamable.h"

class specDescriptor : public specStreamable
{
	//make inline
private:
	QVariant contentValue ;
	quint32 currentLine ;
	bool multiline ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::descriptor ; }
	bool alternativeType(type t) const ;
	void initializeFromString(const QString&, quint32, bool) ;
	bool bothNumeric(const specDescriptor& other) const ;
protected:
	void readAlternative(QDataStream &in, type t) ;
public:
	specDescriptor(QString cont = "",
				quint32 currentLine = 0,
				bool multiline = false) ;
	specDescriptor(const QStringList&,
				quint32 currentLine = 0,
				bool multiline = false) ;
	specDescriptor(double d) ;

	double numericValue() const ;
	QString content(bool full = false) const;
	quint32 activeLine() const ;
	bool isMultiline() const ;
	bool isNumeric() const ;

	void setContent(const QString&) ;
	void setContent(const QStringList& l) ;
	void setContent(const double&) ;
	void setActiveLine(quint32) ;
	void setMultiline(bool on = true) ;
	void setAppropriateContent(const QString&) ;

	specDescriptor& operator= (const double&) ;
	specDescriptor& operator= (const QString&) ;

	bool operator==(const specDescriptor&) const ;
	bool fuzzyComparison(const specDescriptor& other, double tolerance) const ;

	static specDescriptor merge(const QList<specDescriptor>&) ;
	specDescriptor& operator+=(const specDescriptor& other) ;
	specDescriptor operator+(const specDescriptor& other) const ;
	specDescriptor operator*(const double& d) const ;
	operator QString() const ;
};

class specLegacyDescriptor : public specDescriptor
{
	type typeId() const { return myType ; }
	void readFromStream(QDataStream& in);
	type myType ;
public:
	specLegacyDescriptor() ;
};

// Changes from old descriptor:
// dropped "numeric" and "editable" properties.  New
//   - all descriptors are editable
//   - numeric values are extracted as needed.
// Data layout (in file) changed
// core content changed (from QString and int to QVariant and int)
#endif
