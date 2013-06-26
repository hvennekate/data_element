#ifndef SPECLOGENTRYITEM_H
#define SPECLOGENTRYITEM_H

#include "specfolderitem.h"
#include "specdescriptor.h"
#include <QHash>

class specLogEntryItem : public specModelItem
{
protected:
	QHash<QString,specDescriptor> description ; // TODO legacy for specDataItem::operator+=() -> change!
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	bool isNumeric(const QString &key) const ;
	double numericalValue(const QString &key) const ;
private:
	type typeId() const { return specStreamable::logEntry ; }
public:
	explicit specLogEntryItem ( QHash<QString,specDescriptor> description = QHash<QString,specDescriptor>(), specFolderItem* par=0, QString tag="");
	specLogEntryItem(const specLogEntryItem&) ;
	~specLogEntryItem();

	bool isEditable ( QString key ) const;
	bool changeDescriptor ( QString key, QString value ) ;
	QStringList descriptorKeys() const ;
	QIcon decoration() const ;
	spec::descriptorFlags descriptorProperties ( const QString& key ) const ;
	void setDescriptorProperties(const QString &key, spec::descriptorFlags f) ;
	QString descriptor ( const QString &key, bool full = false) const ;
	double descriptorValue(const QString &key) const ;
	bool setActiveLine(const QString &, int) ;
	int activeLine(const QString &key) const ;
	void renameDescriptors(const QMap<QString, QString> &map);
	void deleteDescriptor(const QString &descriptor);
	void dumpDescriptor(QList<specDescriptor> &destination, const QString &key) const;
	void restoreDescriptor(QListIterator<specDescriptor> &origin, const QString &key) ;
};

#endif
