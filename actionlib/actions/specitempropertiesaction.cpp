#include "specitempropertiesaction.h"
#include "specdataitem.h"
#include "specsvgitem.h"
#include "specmetaitem.h"

specItemPropertiesAction::specItemPropertiesAction(QObject *parent) :
	specItemAction(parent)
{
	setIcon(QIcon::fromTheme("document-properties")) ;
	setToolTip(tr("Edit item properties")) ;
	setWhatsThis(tr("Displays an item-specific dialog to let you edit the current item's properties")) ;
	setText(tr("Item properties...")) ;
	setShortcut(tr("Ctrl+i"));
}

specUndoCommand* specItemPropertiesAction::generateUndoCommand()
{
	if (!currentItem) return 0 ;
	return currentItem->itemPropertiesAction(model) ;
}

bool specItemPropertiesAction::specificRequirements()
{
	return (dynamic_cast<specDataItem*>(currentItem)
			|| dynamic_cast<specMetaItem*>(currentItem)
			|| dynamic_cast<specSVGItem*>(currentItem)) ;
}
