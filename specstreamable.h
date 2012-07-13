#ifndef SPECSTREAMABLE_H
#define SPECSTREAMABLE_H

#include <QDataStream>

class specStreamable
{

protected:
	virtual void writeToStream(QDataStream& out) const = 0;
	virtual void readFromStream(QDataStream& in) = 0;
	virtual void writeContents(QDataStream& out) const {Q_UNUSED(out)} ; // write container contents
	virtual void readContents(QDataStream& in) {Q_UNUSED(in)} ; // read container contents
	typedef quint16 type;
	enum streamableType : type
	{
		container = 32768,
		someItem = 1,
	};
	virtual type id() const = 0;
	virtual bool isContainer() const { return false ; }
	virtual specStreamable* factory(const type&) const {return 0; } // to be implemented in parent!
	specStreamable* produceItem(QDataStream& in) const;
private:
	type effectiveId() const ;
	static bool isContainer(const type& t) { return t & container ; }
	static void skipContainer(QDataStream& in) ;
public:
	friend QDataStream& operator<<(QDataStream& out, const specStreamable&) ;
	friend QDataStream& operator>>(QDataStream& in, specStreamable&) ;
};

class specStreamContainer : public specStreamable
{
protected:
	bool isContainer() const { return true ; }
};

QDataStream& operator<<(QDataStream& out, const specStreamable&) ;
QDataStream& operator>>(QDataStream& in, specStreamable&) ;

#endif // SPECSTREAMABLE_H
