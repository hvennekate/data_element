#include "specconductfitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "speceditdescriptorcommand.h"
#include "specmulticommand.h"

specConductFitAction::specConductFitAction(QObject *parent) :
    specItemAction(parent)
{
	setIcon(QIcon(":/doFit.png")) ;
	setToolTip(tr("Conduct fit")) ;
	setWhatsThis(tr("Conducts fits for the selected meta items.")) ;
	setText(tr("Fit")) ;
}

specUndoCommand* specConductFitAction::generateUndoCommand()
{
	specMultiCommand *parentCommand = new specMultiCommand ;
	parentCommand->setText(tr("Conduct fitting")) ;
	parentCommand->setParentObject(model);
	foreach(QModelIndex index, selection)
	{
		specMetaItem *item = dynamic_cast<specMetaItem*>(model->itemPointer(index)) ;
		if (!item) continue;
		if(!(item->getFitCurve())) continue ;
		QString oldDescriptor = item->descriptor(tr("Fit variables"), true) ;
		item->conductFit();
		QString newDescriptor = item->descriptor(tr("Fit variables"), true) ;
		specEditDescriptorCommand *editCommand = new specEditDescriptorCommand(parentCommand) ;
		editCommand->setParentObject(model) ;
		editCommand->setItem(index,tr("Fit variables"), newDescriptor, item->activeLine(tr("Fit variables"))) ;
		item->changeDescriptor(tr("Fit variables"), oldDescriptor) ;
	}
	if (!parentCommand->childCount())
	{
		delete parentCommand ;
		return 0 ;
	}
	return parentCommand ;
}
