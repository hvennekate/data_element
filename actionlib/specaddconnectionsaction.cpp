#include "specaddconnectionsaction.h"
#include "specmetaview.h"
#include "specplotwidget.h"
#include "specaddconnectionscommand.h"

specAddConnectionsAction::specAddConnectionsAction(QObject *parent) :
    specUndoAction(parent)
{
	this->setIcon(QIcon::fromTheme("list-new")) ;
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
	specMetaModel *model = view->model() ;
	qDebug("got model") ;
	specView *serverView = view->getDataView() ;
	qDebug("got server view") ;
	specModel *servermodel = serverView->model() ;
	qDebug("got server model") ;

	QModelIndex target = view->currentIndex() ;
	QModelIndexList servers = serverView->getSelection() ;
	if (!target.isValid() || servers.isEmpty()) return ;
	qDebug("prepared target list") ;

	specAddConnectionsCommand *command = new specAddConnectionsCommand ;
	command->setParentWidget((QWidget*) parent()) ;
	command->setItems(target,servers) ;
	qDebug("prepared command") ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
