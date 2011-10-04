#ifndef SPECLOGENTRYITEM_H
#define SPECLOGENTRYITEM_H

#include "specfolderitem.h"
#include "specdescriptor.h"
#include <QHash>

class specLogEntryItem : public specFolderItem
{
private:
	QHash<QString,specDescriptor> description ;
protected:
	QDataStream& readFromStream ( QDataStream& arg1 );
	QDataStream& writeToStream ( QDataStream& arg1 ) const;
public:
	specLogEntryItem ( QHash<QString,specDescriptor> description, specFolderItem* par=0, QString tag="" );
	~specLogEntryItem();

	bool isEditable ( QString key ) const;
	bool changeDescriptor ( QString key, QString value ) ;
	QStringList descriptorKeys() const ;
	QIcon decoration() const ;
	spec::descriptorFlags descriptorProperties ( const QString& key ) const ;
	QString descriptor ( const QString &key, bool full) const ;
};

#endif
