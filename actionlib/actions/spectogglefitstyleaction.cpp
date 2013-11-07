#include "spectogglefitstyleaction.h"
#include "specmetaitem.h"
#include "spectogglefitstylecommand.h"

specToggleFitStyleAction::specToggleFitStyleAction(QObject* parent) :
	specRequiresMetaItemAction(parent)
{
	setIcon(QIcon(":/styleFit.png")) ;
	setToolTip(tr("Switch styling to/from fit")) ;
	setWhatsThis(tr("Switches the styling options to affect the fit connected with this meta item or the meta item itself.")) ;
	setText(tr("Switch styling to/from fit")) ;
}

specUndoCommand* specToggleFitStyleAction::generateUndoCommand()
{
	specToggleFitStyleCommand* command = new specToggleFitStyleCommand ;
	command->setText(tr("Toggle styling to fit")) ;
	command->setParentObject(model) ;
	command->setItem(currentItem) ;
	return command ;
}
