#include "genericexportaction.h"

genericExportAction::genericExportAction(QObject *parent) :
    specRequiresItemAction(parent)
{
	setIcon(QIcon(":/export.png")) ;
	setToolTip(tr("Export to ASCII")) ;
	setWhatsThis(tr("Export selected items to ASCII file.")) ;
	setText(tr("Export ASCII..."));
	setShortcut(tr("e"));
}

specUndoCommand* genericExportAction::generateUndoCommand()
{
	model->exportData(selection) ;
	return 0 ;
}
