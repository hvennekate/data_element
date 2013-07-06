#ifndef SPECCONNECTIONSACTION_H
#define SPECCONNECTIONSACTION_H

#include "specrequiresitemaction.h"

class specConnectionsAction : public specRequiresMetaItemAction
{
	Q_OBJECT
private:
	bool requirements() ;
protected:
	specView *dataView ;
	specMetaView *metaView ;
public:
	explicit specConnectionsAction(QObject *parent = 0);
	const std::type_info& possibleParent() ;
};

#endif // SPECCONNECTIONSACTION_H
