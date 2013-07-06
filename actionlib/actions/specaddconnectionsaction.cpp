#include "specaddconnectionsaction.h"
#include "specmetaview.h"
#include "specplotwidget.h"
#include "specaddconnectionscommand.h"
#include "specmulticommand.h"

specAddConnectionsAction::specAddConnectionsAction(QObject *parent) :
	specConnectionsAction(parent)
{
	this->setIcon(QIcon(":/toKinetic.png")) ;
	setToolTip(tr("Connect current to selected."));
	setWhatsThis(tr("Connects the current item in the meta dock window with the selected items in the data and meta dock window.\nThis is the essential action for enabling a meta item to obtain data for processing from other items."));
	setText(tr("Connect to selected")) ;
	setShortcut(tr("Ctrl+Alt+c"));
}

bool specAddConnectionsAction::requirements()
{
	if (!specConnectionsAction::requirements()) return false ;
	if (!dataView->model()) return false ;
	QModelIndexList servers = dataView->getSelection() ;
	dataView->model()->expandFolders(servers) ;
	return !servers.empty() ;
}

specUndoCommand* specAddConnectionsAction::generateUndoCommand()
{
	specMultiCommand *command = new specMultiCommand ;
	command->setParentObject(model);
	foreach(specModelItem* item, pointers)
	{
		QModelIndex target = model->index(item) ;
		if (!target.isValid()) continue ;
		specAddConnectionsCommand *connectionsCommand = new specAddConnectionsCommand(command) ;
		command->setItems(target,servers) ;
	}

	command->setText(tr("Connect to data items")) ;
	return command ;
}

