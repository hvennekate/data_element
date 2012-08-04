#include "specaddconnectionsaction.h"
#include "specmetaview.h"
#include "specplotwidget.h"
#include "specaddconnectionscommand.h"

specAddConnectionsAction::specAddConnectionsAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("list-new",QIcon(":/toKinetic.png"))) ;
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
	command->setParentObject((QObject*) parent()) ;
	command->setItems(target,servers) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
