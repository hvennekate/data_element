#include "specaddconnectionsaction.h"
#include "specmetaview.h"
#include "specplotwidget.h"
#include "specaddconnectionscommand.h"

specAddConnectionsAction::specAddConnectionsAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("list-new",QIcon(":/toKinetic.png"))) ;
	setToolTip(tr("Connect current to selected."));
	setWhatsThis(tr("Connects the current item in the meta dock window with the selected items in the data and meta dock window.\nThis is the essential action for enabling a meta item to obtain data for processing from other items."));
}

const std::type_info &specAddConnectionsAction::possibleParent()
{
	return typeid(specMetaView) ;
}

void specAddConnectionsAction::execute()
{
	specMetaView *view = (specMetaView*) parent() ;
	specView *serverView = view->getDataView() ;

	QModelIndex target = view->currentIndex() ;
	QModelIndexList servers = serverView->getSelection() ;
	if (!target.isValid() || servers.isEmpty()) return ;

	specAddConnectionsCommand *command = new specAddConnectionsCommand ;
	command->setParentObject((QObject*) view->model()) ;
	command->setItems(target,servers) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
