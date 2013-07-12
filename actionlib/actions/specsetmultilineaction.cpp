#include "specsetmultilineaction.h"
#include "specdescriptorflagscommand.h"

specSetMultilineAction::specSetMultilineAction(QObject *parent) :
	specRequiresItemAction(parent)
{
	setIcon(QIcon(":/multilineOn.png")) ;
	setToolTip(tr("Toggle showing all lines") );
	setWhatsThis(tr("Switches showing all lines of this descriptor on or off."));
	setText(tr("Show all lines"));
	setCheckable(true) ;
	setShortcut(tr("Ctrl+M"));
}

specUndoCommand *specSetMultilineAction::generateUndoCommand()
{
	if (!currentIndex.isValid()) return 0 ;
	specDescriptorFlagsCommand *command = new specDescriptorFlagsCommand ;
	QString key = model->descriptors()[currentIndex.column()] ; // TODO function descriptor(int) in specModel
	foreach(specModelItem* item, pointers)
		command->addItem(item, key, item->descriptorProperties(key)
				 ^ spec::multiline) ; // XOR
	command->setText(tr("Toggle multiline")) ;
	command->setParentObject(model) ;
	return command ;
}
