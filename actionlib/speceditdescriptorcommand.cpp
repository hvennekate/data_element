#include "speceditdescriptorcommand.h"

specEditDescriptorCommand::specEditDescriptorCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specEditDescriptorCommand::setItem(const QModelIndex &index, QString descriptor,
					QString newContent, int activeLine)
{
	previousContent= newContent ;
	previousActiveLine = activeLine ;
	specDataItem *pointer = dynamic_cast<specDataItem*>(((specView*) parentWidget())->model()->itemPointer(index)) ;
	if (item) delete item ;
	item = 0 ;
	if (!pointer) return ;
	descriptor = desc ;
	item = new specGenealogy(QModelIndexList() << index) ;
}

void specEditDescriptorCommand::undo()
{
	redo() ;
}

void specEditDescriptorCommand::redo()
{
	if (!item) return ;
	specDataItem *pointer = (specDataItem*) (item->items().first()) ;
	QString currentContent = pointer->descriptor(descriptor,true) ;
	int currentLine = descriptor.left(descriptor.indexOf(pointer->descriptor(descriptor,false))).count("\n") ;

	pointer->changeDescriptor(descriptor,previousContent) ;
	pointer->setActiveLine(descriptor,previousActiveLine) ;

	previousContent = currentContent ;
	previousActiveLine = currentLine ;

	specModel *model = item->model() ;
	if (model)
	{
		QModelIndex index = model->index(pointer)
		emit model->dataChanged(index,index);
	}
}
