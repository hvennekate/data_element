#include "specconnectionsaction.h"

specConnectionsAction::specConnectionsAction(QObject *parent) :
	specRequiresMetaItemAction(parent),
	dataView(0),
	metaView(0)
{
}

bool specConnectionsAction::requirements()
{
	metaView = qobject_cast<specMetaView*>(view) ;
	if (!metaView) return false ;
	dataView = metaView->getDataView() ;
	return dataView ;
}

const std::type_info& specSelectConnectedAction::possibleParent()
{
	return typeid(specMetaView) ;
}
