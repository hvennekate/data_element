#ifndef SPECDELETEDESCRIPTORCOMMAND_H
#define SPECDELETEDESCRIPTORCOMMAND_H

#include "specundocommand.h"
#include "specdescriptor.h"

class specDeleteDescriptorCommand : public specUndoCommand
{
private:
	QList<specDescriptor> contents ;
	QString key ;
	qint16 position ;
	spec::descriptorFlags flags ;
	void writeCommand(QDataStream& out) const ;
	void readCommand(QDataStream& in) ;
	void doIt() ;
	void undoIt() ;
	type typeId() const { return specStreamable::deleteDescriptorCommandId ; }
	void parentAssigned();
public:
	specDeleteDescriptorCommand(specUndoCommand* parent =  0, QString key = "");
};

#endif // SPECDELETEDESCRIPTORCOMMAND_H
