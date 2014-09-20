#include "specsetmultilineaction.h"
#include "specmultilinecommand.h"

specSetMultilineAction::specSetMultilineAction(QObject* parent) :
	specRequiresItemAction(parent)
{
	setIcon(QIcon(":/multiline.png")) ;
	setToolTip(tr("Toggle showing all lines"));
	setWhatsThis(tr("Switches showing all lines of this descriptor on or off."));
	setText(tr("Show all lines"));
	setCheckable(true) ;
	setShortcut(tr("Ctrl+M"));
}

specUndoCommand* specSetMultilineAction::generateUndoCommand()
{
	if(!currentIndex.isValid()) return 0 ;
	specMultiLineCommand* command = new specMultiLineCommand(model->descriptors() [currentIndex.column()]) ;
	command->setParentObject(model) ;
	command->setItems(pointers) ;
	command->setText(tr("Toggle multiline")) ;
	return command ;
}

bool specSetMultilineAction::specificCheckRequirements()
{
	if (!model) return false ;
	if (!currentIndex.isValid()) return false ;
	return model->data(currentIndex, spec::MultiLineRole).toBool() ;
}
