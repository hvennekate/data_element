#ifndef CHANGEMODELDESCRIPTORPROPERTIESCOMMAND_H
#define CHANGEMODELDESCRIPTORPROPERTIESCOMMAND_H

#include "specundocommand.h"
#include "names.h"

class changeModelDescriptorPropertiesCommand : public specUndoCommand
{
	QString name ;
	spec::descriptorFlags flags ;

	void doIt();
	void undoIt();
	type typeId() const { return specStreamable::changeModelDescriptorPropertiesCommandId ; }
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
public:
	explicit changeModelDescriptorPropertiesCommand(specUndoCommand* parent = 0,
							QString descriptor = QString(),
							spec::descriptorFlags flags = spec::def);
	void setDescriptor(QString d) ;
	void setFlags(spec::descriptorFlags f) ;
};

#endif // CHANGEMODELDESCRIPTORPROPERTIESCOMMAND_H
