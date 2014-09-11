#include "changemodeldescriptorpropertiescommand.h"
#include "specmodel.h"

void changeModelDescriptorPropertiesCommand::doIt()
{
	specModel* myModel = model() ; // TODO macro oder aehnliches (-> view)
	if (!myModel) return ;
	myModel->signalBeginReset();
	spec::descriptorFlags f = myModel->descriptorProperties(name) ;
	myModel->setDescriptorProperties(flags, name) ;
	flags = f ;
}

void changeModelDescriptorPropertiesCommand::undoIt()
{
	doIt() ;
}

void changeModelDescriptorPropertiesCommand::writeCommand(QDataStream &out) const
{
	out << name << (quint8) flags ;
}

void changeModelDescriptorPropertiesCommand::readCommand(QDataStream &in)
{
	quint8 f ;
	in >> name >> f ;
	flags = (spec::descriptorFlags) f ;
}

changeModelDescriptorPropertiesCommand::changeModelDescriptorPropertiesCommand
	(specUndoCommand *parent, QString d, spec::descriptorFlags f)
	: specUndoCommand(parent),
	  name(d),
	  flags(f)
{
}

void changeModelDescriptorPropertiesCommand::setDescriptor(QString d)
{
	name = d ;
}

void changeModelDescriptorPropertiesCommand::setFlags(spec::descriptorFlags f)
{
	flags = f ;
}


