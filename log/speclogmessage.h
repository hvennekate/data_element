#ifndef SPECLOGMESSAGE_H
#define SPECLOGMESSAGE_H

#include "speclogentryitem.h"

class specLogMessage : public specLogEntryItem
{
private:
	type typeId() const { return specStreamable::sysEntry ; }
public:
	specLogMessage(QHash<QString, specDescriptor> description = QHash<QString, specDescriptor>(),
		       specFolderItem* par = 0,
		       QString tag = "") ;
	~specLogMessage();

	bool changeDescriptor(QString key, QString value) ;
	bool isEditable(QString key) const;
	QIcon decoration() const;
};

#endif
