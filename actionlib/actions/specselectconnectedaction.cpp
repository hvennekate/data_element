#include "specselectconnectedaction.h"
#include "specmetaview.h"
#include "specmetaitem.h"
#include "specviewstate.h"

specSelectConnectedAction::specSelectConnectedAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon(":/fromKinetic.png"));
	setText(tr("Select connected items"));
	setWhatsThis(tr("Selects the items connected to the current meta item.")) ;
	setToolTip(tr("Select connected items"));
	setShortcut(tr("Ctrl+s"));
}

bool specSelectConnectedAction::requirements()
{
	return qobject_cast<specMetaView*>(view) && dynamic_cast<specMetaItem*>(currentItem) ;
}

const std::type_info& specSelectConnectedAction::possibleParent()
{
	return typeid(specMetaView) ;
}

specUndoCommand *specSelectConnectedAction::generateUndoCommand()
{
	specMetaView *metaView = qobject_cast<specMetaView*>(view) ;
	specMetaItem *currentMetaItem = dynamic_cast<specMetaItem*>(currentItem) ;
	if (! metaView || !currentMetaItem) return 0 ;
	QModelIndexList dataItems, metaItems ;
	currentMetaItem->connectedItems(dataItems, metaItems) ;
	metaView->selectionModel()->select(specViewState::indexesToSelection(metaItems,(specModel*) metaView->model()), QItemSelectionModel::Select) ;
	metaView->getDataView()->selectionModel()->select(specViewState::indexesToSelection(dataItems, metaView->getDataView()->model()), QItemSelectionModel::ClearAndSelect) ;
	return 0 ;
}
