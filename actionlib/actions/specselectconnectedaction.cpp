#include "specselectconnectedaction.h"
#include "specmetaview.h"
#include "specmetaitem.h"
#include "specviewstate.h"

specSelectConnectedAction::specSelectConnectedAction(QObject* parent) :
	specConnectionsAction(parent)
{
	setIcon(QIcon(":/fromKinetic.png"));
	setText(tr("Select connected items"));
	setWhatsThis(tr("Selects the items connected to the current meta item.")) ;
	setToolTip(tr("Select connected items"));
	setShortcut(tr("Ctrl+s"));
}


specUndoCommand* specSelectConnectedAction::generateUndoCommand()
{
	foreach(specModelItem * pointer, pointers)
	{
		specMetaItem* metaPointer = (specMetaItem*) pointer ;
		QModelIndexList dataItems, metaItems ;
		metaPointer->connectedItems(dataItems, metaItems) ;
		metaView->selectionModel()->select(specViewState::indexesToSelection(metaItems, (specModel*) metaView->model()), QItemSelectionModel::Select) ;
		dataView->selectionModel()->select(specViewState::indexesToSelection(dataItems, metaView->getDataView()->model()), QItemSelectionModel::ClearAndSelect) ;
	}
	return 0 ;
}
