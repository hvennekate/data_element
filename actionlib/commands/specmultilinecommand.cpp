#include "specmultilinecommand.h"

void specMultiLineCommand::doIt()
{
	foreach(specModelItem* item, itemPointers())
		item->setMultiline(descriptorName,
				   !item->isMultiline(descriptorName));
	signalModelChanged();
}

void specMultiLineCommand::undoIt()
{
	doIt();
}

void specMultiLineCommand::writeCommandData(QDataStream &out) const
{
	out << descriptorName ;
}

void specMultiLineCommand::readCommandData(QDataStream &in)
{
	in >> descriptorName ;
	generateDescription();
}

void specMultiLineCommand::generateDescription()
{
	setText(QObject::tr("Toggle multiline for descriptor \"")
		+ descriptorName
		+ QObject::tr("\".")) ;
}

QString specMultiLineCommand::description() const
{
	return QString() ;
}

void specMultiLineCommand::signalModelChanged()
{
	if (!model()) return ;
	int column = model()->descriptors().indexOf(descriptorName) ;
	foreach(specModelItem* item, itemPointers())
		model()->signalChanged(model()->index(item, column));
}

specMultiLineCommand::specMultiLineCommand(QString d, specUndoCommand *parent)
	: specMultipleItemCommand(parent)
{
	setDescriptor(d) ;
	generateDescription();
}

specMultiLineCommand::specMultiLineCommand(specUndoCommand *parent)
	: specMultipleItemCommand(parent)
{
	generateDescription();
}

void specMultiLineCommand::setDescriptor(const QString &d)
{
	descriptorName = d ;
	generateDescription();
}

void specDescriptorFlagsCommand::readCommand(QDataStream &in)
{
	in >> oldItems >> descriptorName ;
	generateDescription();
}

void specDescriptorFlagsCommand::writeCommand(QDataStream &out) const
{
	out << oldItems << descriptorName ;
}

void specDescriptorFlagsCommand::doIt()
{
	if (myType == multilineCommandId)
	{
		specMultiLineCommand::doIt() ;
		return ;
	}
	QList<specModelItem*> itemList ;
	foreach(itemPropertyPair p, oldItems)
	{
		bool turnMultilineOn = p.second & 4 ; // spec::multiline = 4
		specModelItem* item = p.first.firstItem() ;
		if (!item) continue ;
		if (item->isMultiline(descriptorName) == turnMultilineOn)
			continue ; // Nothing to change here
		item->setMultiline(descriptorName, turnMultilineOn);
		itemList << item ; // and from here on, business as usual.
	}

	oldItems.clear() ;
	setItems(itemList) ;
	myType = multilineCommandId ;
	signalModelChanged();
}

void specDescriptorFlagsCommand::parentAssigned()
{
	for (itemsContainer::iterator i = oldItems.begin() ; i != oldItems.end() ; ++i)
		i->first.setModel(model());
}

specDescriptorFlagsCommand::specDescriptorFlagsCommand(specUndoCommand *parent)
	: specMultiLineCommand("",parent),
	  myType(descriptorFlagsCommand)
{
}

