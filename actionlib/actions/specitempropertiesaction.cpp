#include "specitempropertiesaction.h"

specItemPropertiesAction::specItemPropertiesAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon::fromTheme("document-properties")) ;
	setToolTip(tr("Edit item properties")) ;
	setWhatsThis(tr("Displays an item-specific dialog to let you edit the current item's properties")) ;
}

specUndoCommand* specItemPropertiesAction::generateUndoCommand()
{
	if (!currentItem) return 0 ;
	return currentItem->itemPropertiesAction(model) ;
}
