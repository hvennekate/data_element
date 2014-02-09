#include "specaddconnectionsaction.h"
#include "specmetaview.h"
#include "specplotwidget.h"
#include "specaddconnectionscommand.h"
#include "specmulticommand.h"
#include "specmetaitem.h"

specAddConnectionsAction::specAddConnectionsAction(QObject* parent) :
	specConnectionsAction(parent),
	dataViewRequirement(false),
	metaViewRequirement(false),
	changing(false)
{
	this->setIcon(QIcon(":/toKinetic.png")) ;
	setToolTip(tr("Connect current to selected."));
	setWhatsThis(tr("Connects the current item in the meta dock window with the selected items in the data and meta dock window.\nThis is the essential action for enabling a meta item to obtain data for processing from other items."));
	setText(tr("Connect to selected")) ;
	setShortcut(tr("Ctrl+Alt+c"));

	specMetaView* view = dynamic_cast<specMetaView*>(parent) ;
	connect(this, SIGNAL(changed()), this, SLOT(changedByMetaView())) ;

	if(view && view->getDataView() && view->getDataView()->selectionModel())
		connect(view->getDataView()->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
			this, SLOT(changeByDataView())) ;
}

void specAddConnectionsAction::changedByMetaView()
{
	if(changing) return ;
	metaViewRequirement = isEnabled() ;
	checkRequirements() ;
}

void specAddConnectionsAction::changeByDataView()
{
	if (!requirements()) return ;
	if(!dataView) return ;
	dataViewRequirement = dataView->model() && !dataView->getSelection().isEmpty() ;
	checkRequirements() ;
}

void specAddConnectionsAction::checkRequirements()
{
	changing = true ;
	setEnabled(metaViewRequirement && dataViewRequirement);
	changing = false ;
}

specUndoCommand* specAddConnectionsAction::generateUndoCommand()
{
	specMultiCommand* command = new specMultiCommand ;
	command->setParentObject(model);
	QList<specModelItem*> originalSelection = dataView->model()->pointerList(dataView->getSelection()) ;
	foreach(specModelItem * item, pointers)
	(new specAddConnectionsCommand(command))
	->setItems(dynamic_cast<specMetaItem*>(item), originalSelection) ;

	command->setText(tr("Connected ")
			 + QString::number(pointers.size())
			 + tr(" meta items to ")
			 + QString::number(originalSelection.size())
			 + tr(" data items")) ;
	return command ;
}

