#include "genericexportaction.h"

genericExportAction::genericExportAction(QObject *parent) :
    specUndoAction(parent)
{
	setIcon(QIcon(":/export.png")) ;
	setToolTip(tr("Export to ASCII")) ;
	setWhatsThis(tr("Export selected items to ASCII file.")) ;
}

const std::type_info &genericExportAction::possibleParent()
{
	return typeid(specView) ;
}

void genericExportAction::execute()
{
	specView* view = qobject_cast<specView*>(parentWidget()) ;
	if (!view) return ;
	specModel* model = view->model() ;
	if (!model) return ;

	QModelIndexList indexes(view->getSelection()) ;
	model->exportData(indexes) ;
}
