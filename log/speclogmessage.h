#ifndef SPECLOGMESSAGE_H
#define SPECLOGMESSAGE_H

#include <specdataitem.h>

class specLogMessage : public specModelItem
{
private:
	type typeId() const { return specStreamable::sysEntry ; }
public:
	specLogMessage(specFolderItem* par=0, QString tag="");
	~specLogMessage();

	bool changeDescriptor ( QString key, QString value ) { return false ; }
	bool isEditable ( QString key ) const;
	QIcon decoration() const;
	spec::descriptorFlags descriptorProperties ( const QString& key ) const;
};

#endif
