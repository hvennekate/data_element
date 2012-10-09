#include "spectogglefitstyleaction.h"
#include "specmetaitem.h"
#include "spectogglefitstylecommand.h"

specToggleFitStyleAction::specToggleFitStyleAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon(":/toggleFit.png")) ;
	setToolTip(tr("Switch styling to fit")) ;
	setWhatsThis(tr("Switches the styling options to affect the fit connected with this meta item.")) ;
	setText(tr("Switch styling to fit")) ;
}

specUndoCommand* specToggleFitStyleAction::generateUndoCommand()
{
	specMetaItem *item = dynamic_cast<specMetaItem*>(currentItem) ; // TODO template itemAction
	if (!item) return 0 ;
	specToggleFitStyleCommand *command = new specToggleFitStyleCommand ;
	command->setText(tr("Toggle styling to fit")) ;
	command->setParentObject(model) ;
	command->setup(currentIndex) ;
	return command ;
}
