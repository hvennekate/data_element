#include "speceditdescriptorcommand.h"
#include "specmodelitem.h"
#include <QQueue>

specEditDescriptorCommand::specEditDescriptorCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

void specEditDescriptorCommand::setItem(const QModelIndex &index, QString desc,
					QString newContent, int activeLine)
{
	previousContent.clear();
	previousActiveLine.clear();
	previousContent << newContent ;
	previousActiveLine << activeLine ;
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
	if (!pointer || previousActiveLine.isEmpty() || previousContent.isEmpty()) return ;
	QVector<int>::iterator previousActiveLineIterator(previousActiveLine.begin()) ;
	QStringList::iterator previousContentIterator(previousContent.begin()) ;

	QStringList newContent ;
	QVector<int> newActiveLines ;

	QQueue<specModelItem*> items ;
	items.enqueue(pointer) ;
	while(!items.isEmpty())
	{
		pointer = items.dequeue() ;
		QString currentContent = pointer->descriptor(descriptor,true) ;
		int currentLine = pointer->activeLine(descriptor) ;

		if (pointer->changeDescriptor(descriptor, *previousContentIterator))
		{
			pointer->setActiveLine(descriptor, *previousActiveLineIterator) ;
			newContent << currentContent ;
			newActiveLines << currentLine ;
			if (previousActiveLineIterator+1 != previousActiveLine.end())
				++ previousActiveLineIterator;
			if (previousContentIterator+1 != previousContent.end())
				++ previousContentIterator ;

		}
		else if (pointer->isFolder())
		{
			specFolderItem *folder = dynamic_cast<specFolderItem*>(pointer) ;
			if (!folder) continue ;
			for (int i = 0 ; i < folder->children() ; ++i)
				items.enqueue(folder->child(i)) ;
		}
	}

	if (item.model()) // TODO: maybe implement specModel::index(specModelItem*, QString descriptor)
		item.model()->signalChanged(item.firstIndex()) ;
	previousContent.swap(newContent) ;
	previousActiveLine.swap(newActiveLines);
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
