#include "specaddfitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "specexchangefitcurvecommand.h"
#include "specmulticommand.h"

specAddFitAction::specAddFitAction(QObject *parent) :
	specRequiresMetaItemAction(parent)
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
	foreach(specModelItem* pointer, pointers)
	{
		specMetaItem *item = (specMetaItem*) pointer ; // TODO template itemAction
		if (item->getFitCurve()) continue ;
		(new specExchangeFitCurveCommand(parentCommand))
				->setup(pointer, new specFitCurve) ;
		// TODO changed this to inherit parentObject from parent -> check other places for redundancy: command->setParentObject(model) ;
	}

	if (!parentCommand->childCount())
	{
		delete parentCommand ;
		return 0 ;
	}
	return parentCommand ;
}
