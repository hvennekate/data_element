#include "specremovefitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "specexchangefitcurvecommand.h"

specRemoveFitAction::specRemoveFitAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon(":/removeFit.png")) ;
	setToolTip(tr("Remove fit")) ;
	setWhatsThis(tr("Removes the fit from the current item.")) ;
	setText(tr("Remove fit")) ;
	setShortcut(tr("Ctrl+Shift+F"));
}

specUndoCommand* specRemoveFitAction::generateUndoCommand()
{
	specMetaItem *item = dynamic_cast<specMetaItem*>(currentItem) ; // TODO template itemAction
	if (!item) return 0 ;
	if (!item->getFitCurve()) return 0 ;
	specExchangeFitCurveCommand *command = new specExchangeFitCurveCommand ;
	command->setText(tr("Remove fit curve"))  ;
	command->setParentObject(model) ;
	command->setup(currentIndex, 0) ;
	return command ;
}
