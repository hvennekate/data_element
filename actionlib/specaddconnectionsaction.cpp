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
	qDebug("adding connections") ;
	specMetaView *view = (specMetaView*) parent() ;
	qDebug("got view") ;
	specView *serverView = view->getDataView() ;
	qDebug("got server view") ;

	QModelIndex target = view->currentIndex() ;
	QModelIndexList servers = serverView->getSelection() ;
	if (!target.isValid() || servers.isEmpty()) return ;
	qDebug("prepared target list") ;

	specAddConnectionsCommand *command = new specAddConnectionsCommand ;
	command->setParentObject((QWidget*) parent()) ;
	command->setItems(target,servers) ;
	qDebug("prepared command") ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
