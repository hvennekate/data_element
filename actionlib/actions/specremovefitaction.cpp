#include "specremovefitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "specexchangefitcurvecommand.h"
#include "specmulticommand.h"

specRemoveFitAction::specRemoveFitAction(QObject* parent) :
	specFitAction(parent)
{
	setIcon(QIcon(":/removeFit.png")) ;
	setToolTip(tr("Remove fit")) ;
	setWhatsThis(tr("Removes the fit from the current item.")) ;
	setText(tr("Remove fit")) ;
	setShortcut(tr("Ctrl+Shift+F"));
}

specUndoCommand* specRemoveFitAction::generateUndoCommand()
{
	specMultiCommand* parentCommand = new specMultiCommand ;
	parentCommand->setText(tr("Remove fit curve"));
	parentCommand->setParentObject(model) ;
	foreach(specModelItem * modelItem, pointers)
	{
		specMetaItem* item = dynamic_cast<specMetaItem*>(modelItem) ;
		if(!item) continue ;
		if(!item->getFitCurve()) continue;
		(new specExchangeFitCurveCommand(parentCommand))
		->setup(item, 0) ;
	}

	if(!parentCommand->childCount())  // TODO per virtual functions in parent Klasse unterbringen
	{
		delete parentCommand ;
		return 0 ;
	}

	return parentCommand ;
}
