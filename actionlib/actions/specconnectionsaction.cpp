#include "specconnectionsaction.h"
#include "specmetaview.h"

specConnectionsAction::specConnectionsAction(QObject *parent) :
	specRequiresMetaItemAction(parent),
	dataView(0),
	metaView(0)
{
}

bool specConnectionsAction::specificRequirements()
{
	return (metaView = qobject_cast<specMetaView*>(view))
			&& (dataView = metaView->getDataView()) ;
}

const std::type_info& specConnectionsAction::possibleParent()
{
	return typeid(specMetaView) ;
}
