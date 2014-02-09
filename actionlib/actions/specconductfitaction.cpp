#include "specconductfitaction.h"
#include "specmetaitem.h"
#include "specfitcurve.h"
#include "speceditdescriptorcommand.h"
#include "specmulticommand.h"

specConductFitAction::specConductFitAction(QObject* parent) :
	specFitAction(parent)
{
	setIcon(QIcon(":/doFit.png")) ;
	setToolTip(tr("Conduct fit")) ;
	setWhatsThis(tr("Conducts fits for the selected meta items.")) ;
	setText(tr("Fit")) ;
	setShortcut(tr("Ctrl+Alt+f"));
}

specUndoCommand* specConductFitAction::generateUndoCommand()
{
	specMultiCommand* parentCommand = new specMultiCommand ;
	parentCommand->setText(tr("Conduct fitting")) ;
	parentCommand->setParentObject(model);
	foreach(specModelItem * modelItem, pointers)
	{
		specMetaItem* item = dynamic_cast<specMetaItem*>(modelItem) ;
		if(!item) continue;
		if(!(item->getFitCurve())) continue ;
		QString oldDescriptor = item->descriptor(tr("Fit variables"), true) ;
		int oldActiveLine = item->activeLine(tr("Fit variables")) ;
		item->conductFit();
		QString newDescriptor = item->descriptor(tr("Fit variables"), true) ;
		specEditDescriptorCommand* editCommand = new specEditDescriptorCommand(parentCommand) ;
		editCommand->setItem(item, tr("Fit variables"), newDescriptor, item->activeLine(tr("Fit variables"))) ;
		item->changeDescriptor(tr("Fit variables"), oldDescriptor) ;
		item->setActiveLine(tr("Fit variables"), oldActiveLine) ;
	}
	return parentCommand ;
}
