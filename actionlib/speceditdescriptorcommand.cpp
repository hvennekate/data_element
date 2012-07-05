#include "speceditdescriptorcommand.h"

specEditDescriptorCommand::specEditDescriptorCommand(specUndoCommand *parent)
	: specUndoCommand(parent),
	  item(0)
{
}

void specEditDescriptorCommand::setItem(const QModelIndex &index, QString desc,
					QString newContent, int activeLine)
{
	previousContent= newContent ;
	previousActiveLine = activeLine ;
	specDataItem *pointer = dynamic_cast<specDataItem*>(((specModel*) parentObject())->itemPointer(index)) ;
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
	int currentLine = pointer->activeLine(descriptor) ;

	pointer->changeDescriptor(descriptor,previousContent) ;
	pointer->setActiveLine(descriptor,previousActiveLine) ;

	previousContent = currentContent ;
	previousActiveLine = currentLine ;

	specModel *model = item->model() ;
	if (model)
	{
		QModelIndex index = model->index(pointer) ;
		model->signalChanged(index) ;
	}
}

QDataStream& specEditDescriptorCommand::write(QDataStream &out) const
{
	return item->write(out << previousContent << descriptor << previousActiveLine) ;
}

QDataStream& specEditDescriptorCommand::read(QDataStream &in)
{
	if (item) delete item ;
	item = new specGenealogy(((specModel*) parentObject()),
				 in >> previousContent >> descriptor >> previousActiveLine) ;
	return in ;
}

bool specEditDescriptorCommand::mergeable(const specUndoCommand *other)
{
	return id() == other->id()
			&& ((specEditDescriptorCommand*) other)->previousContent == previousContent
			&& *(((specEditDescriptorCommand*) other)->item) == *item
			&& ((specEditDescriptorCommand*) other)->descriptor == descriptor ;
}

bool specEditDescriptorCommand::mergeWith(const QUndoCommand *other)
{
	const specEditDescriptorCommand *p = dynamic_cast<const specEditDescriptorCommand*>(other) ;
	if (!p || !mergeable(p)) return false ;
	previousActiveLine = p->previousActiveLine ;
}