#ifndef SPECLOGENTRYITEM_H
#define SPECLOGENTRYITEM_H

#include "specfolderitem.h"
#include "specdescriptor.h"
#include <QHash>

class specLogEntryItem : public specModelItem
{
protected:
	QHash<QString, specDescriptor> description ; // TODO legacy for specDataItem::operator+=() -> change!
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
private:
	type typeId() const { return specStreamable::logEntry ; }
public:
	explicit specLogEntryItem(QHash<QString,
				  specDescriptor> description = QHash<QString, specDescriptor>(),
				  specFolderItem* par = 0,
				  QString tag = "");
	specLogEntryItem(const specLogEntryItem&) ;
	~specLogEntryItem();

	void changeDescriptor(const QString& key, QString value) ;
	QStringList descriptorKeys() const ;
	QIcon decoration() const ;
	specDescriptor getDescriptor(const QString &key) const ;
	void setActiveLine(const QString&, quint32) ;
	void setMultiline(const QString&key, bool on = true) ;
	void renameDescriptors(const QMap<QString, QString>& map);
	void deleteDescriptor(const QString& descriptor);
	void dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const;
	void restoreDescriptor(QListIterator<specDescriptor>& origin, const QString& key) ;
};

#endif
