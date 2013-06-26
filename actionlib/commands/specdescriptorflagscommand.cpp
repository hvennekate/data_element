#include "specdescriptorflagscommand.h"

specDescriptorFlagsCommand::specDescriptorFlagsCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

void specDescriptorFlagsCommand::setItems(const QModelIndexList &list, QString Key, spec::descriptorFlags f)
{
	key = Key ;
	items.clear();
	if (list.isEmpty()) return ;
	foreach(QModelIndex index, list)
		items << itemPropertyPair(specGenealogy(index), f) ;
}

void specDescriptorFlagsCommand::addItem(const QModelIndex& index, QString Key, spec::descriptorFlags f)
{
	if (!index.isValid()) return ;
	key = Key ;
	items << itemPropertyPair(specGenealogy(index), f) ;
}

void specDescriptorFlagsCommand::undoIt()
{
	doIt() ;
}

void specDescriptorFlagsCommand::doIt()
{
	for (itemsContainer::iterator i = items.begin() ; i != items.end() ; ++i)
	{
		specModelItem* item = i->first.firstItem() ;
		if (!item) continue ;
		spec::descriptorFlags temp = item->descriptorProperties(key) ;
		item->setDescriptorProperties(key, spec::descriptorFlags(i->second)) ;
		i->second = temp ;
		if (specModel* model = i->first.model())
			model->signalChanged(model->index(item, model->descriptors().indexOf(key))) ;
	}
}

void specDescriptorFlagsCommand::parentAssigned()
{
	specModel *model = qobject_cast<specModel*>(parentObject()) ;
	for (itemsContainer::iterator i = items.begin() ; i != items.end() ; ++i)
		i->first.setModel(model) ;
}

void specDescriptorFlagsCommand::writeCommand(QDataStream &out) const
{
	out << items << key ;
}

void specDescriptorFlagsCommand::readCommand(QDataStream &in)
{
	in >> items >> key ;
}
