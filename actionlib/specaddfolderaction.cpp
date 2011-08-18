#include "specaddfolderaction.h"
#include "specdataview.h"

specAddFolderAction::specAddFolderAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("folder-new"));
}


const std::type_info &specAddFolderAction::possibleParent()
{
	return typeid(specDataView) ;
}

void specAddFolderAction::execute()
{
	specDataView *currentView = (specDataView*) parent() ;
	QModelIndex parent = currentView->currentIndex() ;

}
