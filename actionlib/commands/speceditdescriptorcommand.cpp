#include "speceditdescriptorcommand.h"
#include "specmodelitem.h"

specEditDescriptorCommand::specEditDescriptorCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

void specEditDescriptorCommand::setItem(const QModelIndex &index, QString desc,
					QString newContent, int activeLine)
{
	previousContent= newContent ;
	previousActiveLine = activeLine ;
	specModelItem *pointer = ((specModel*) (index.model()))->itemPointer(index) ;
	item.setModel(0); // TODO invalidate item
	if (!pointer) return ;
	descriptor = desc ;
	item = specGenealogy(index) ;
}

void specEditDescriptorCommand::undoIt()
{
	doIt() ;
}

void specEditDescriptorCommand::parentAssigned()
{
	item.setModel(qobject_cast<specModel*>(parentObject()));
}

void specEditDescriptorCommand::doIt()
{
	specModelItem *pointer = item.firstItem() ;
	if (!pointer) return ;
	QString currentContent = pointer->descriptor(descriptor,true) ;
	int currentLine = pointer->activeLine(descriptor) ;

	pointer->changeDescriptor(descriptor,previousContent) ;
	pointer->setActiveLine(descriptor,previousActiveLine) ;

	previousContent = currentContent ;
	previousActiveLine = currentLine ;

	specModel *model = item.model() ;
	if (model)
	{ // TODO: maybe implement specModel::index(specModelItem*, QString descriptor)
		QModelIndex index = model->index(pointer, model->descriptors().indexOf(descriptor)) ;
		model->signalChanged(index) ;
	}
}

void specEditDescriptorCommand::writeCommand(QDataStream &out) const
{
	out << previousContent
	    << descriptor
	    << previousActiveLine
	    << item ;
}

void specEditDescriptorCommand::readCommand(QDataStream &in)
{
	in >> previousContent
	   >> descriptor
	   >> previousActiveLine
	   >> item ;
}

//bool specEditDescriptorCommand::mergeable(const specUndoCommand *other)
//{
//	return id() == other->id()
//			&& ((specEditDescriptorCommand*) other)->previousContent == previousContent
//			&& *(((specEditDescriptorCommand*) other)->item) == *item
//			&& ((specEditDescriptorCommand*) other)->descriptor == descriptor ;
//}

//bool specEditDescriptorCommand::mergeWith(const QUndoCommand *other)
//{
//	const specEditDescriptorCommand *p = dynamic_cast<const specEditDescriptorCommand*>(other) ;
//	if (!p || !mergeable(p)) return false ;
//	previousActiveLine = p->previousActiveLine ;
//	return true ;
//}
