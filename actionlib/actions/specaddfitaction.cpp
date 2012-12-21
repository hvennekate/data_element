#include "specaddfitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "specexchangefitcurvecommand.h"
#include "specmulticommand.h"

specAddFitAction::specAddFitAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon(":/addFit.png")) ;
	setToolTip(tr("Add fit")) ;
	setWhatsThis(tr("Adds a fit to the current item.")) ;
	setText(tr("Add fit")) ;
	setShortcut(tr("Ctrl+F"));
}

specUndoCommand* specAddFitAction::generateUndoCommand()
{
    specMultiCommand *parentCommand = new specMultiCommand ;
    parentCommand->setText(tr("Add fit curve")) ;
    parentCommand->setParentObject(model) ;
    foreach(QModelIndex index, selection)
    {
        specMetaItem *item = dynamic_cast<specMetaItem*>(model->itemPointer(index)) ; // TODO template itemAction
        if (!item) continue ;
        if (item->getFitCurve()) continue ;
        specExchangeFitCurveCommand *command = new specExchangeFitCurveCommand(parentCommand) ;
        // TODO changed this to inherit parentObject from parent -> check other places for redundancy: command->setParentObject(model) ;
        command->setup(currentIndex, new specFitCurve) ;
    }

    if (!parentCommand->childCount())
    {
        delete parentCommand ;
        return 0 ;
    }
    return parentCommand ;
}
