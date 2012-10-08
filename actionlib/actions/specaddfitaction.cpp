#include "specaddfitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "specexchangefitcurvecommand.h"

specAddFitAction::specAddFitAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon(":/addFit.png")) ;
	setToolTip(tr("Add fit")) ;
	setWhatsThis(tr("Adds a fit to the currently selected item.")) ;
	setText(tr("Add fit")) ;
}

specUndoCommand* specAddFitAction::generateUndoCommand()
{
	specMetaItem *item = dynamic_cast<specMetaItem*>(currentItem) ; // TODO template itemAction
	if (!item) return 0 ;
	if (item->getFitCurve()) return 0 ;
	specExchangeFitCurveCommand *command = new specExchangeFitCurveCommand ;
	command->setText(tr("Add fit curve"))  ;
	command->setParentObject(model) ;
	command->setup(currentIndex, new specFitCurve) ;
	return command ;
}
